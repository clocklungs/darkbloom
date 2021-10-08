#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

#include "object.hh"
#include "pathfinding.hh"
#include "script.hh"
#include "video.hh"
#include "world.hh"

/*********************************************************************
Object constructor
  initializes the object's data to known values
*********************************************************************/
Object::Object() {
    name = "";
    x = 0;
    y = 0;
    visible = false;
    persistent = false;
    active = false;
    zindex = 0;
    offset_x = 0;
    offset_y = 0;
    image = NULL;
    frame_size.x = 0;
    frame_size.y = 0;
    frame_size.w = 0;
    frame_size.h = 0;
    a_script = "";
    c_script = "";
    collidable = NONE;
    speed = WALK;
    normal_speed = speed;
    step = 0;
    direction = DOWN;
}

/*********************************************************************
Object destructor
  clean up after the object
*********************************************************************/
Object::~Object() {
    if (image != NULL) {
        SDL_FreeSurface(image);
        image = NULL;
    }
}

/*********************************************************************
Object::load()
  initializes the object from a data file
*********************************************************************/
bool Object::load(const char *filename) {
    FILE *in;
    char buffer[100]; // don't shrink this w/ out adjusting fscanfs below
    int temp;
    int line;

    // printf("LOADING OBJECT: %s\n", filename);
    /*open data file (in text mode) for reading*/
    in = fopen(filename, "rt");
    if (in == NULL) {
        fprintf(stderr, "%s: ", filename);
        perror("error");
        return false;
    }

    line = 1;
    while (true) {
        if (fscanf(in, "name: %99s\n", buffer) != 1)
            break;
        line++;
        name = buffer;

        if (fscanf(in, "visible: %i\n", &temp) != 1)
            break;
        line++;
        visible = temp;

        if (fscanf(in, "persistent: %i\n", &temp) != 1)
            break;
        line++;
        persistent = temp;

        if (fscanf(in, "active: %i\n", &temp) != 1)
            break;
        line++;
        active = temp;

        if (fscanf(in, "zindex: %f\n", &zindex) != 1)
            break;
        line++;

        if (fscanf(in, "offset_x: %i\n", &offset_x) != 1)
            break;
        line++;

        if (fscanf(in, "offset_y: %i\n", &offset_y) != 1)
            break;
        line++;

        if (fscanf(in, "image: %99s\n", buffer) != 1)
            break;
        /*try to load image, if any*/
        if (strcmp(buffer, "NONE")) {
            image = IMG_Load(buffer);
            if (image == NULL) {
                fprintf(stderr, "%s: %i: %s\n", filename, line, IMG_GetError());
                break;
            }
        }
        line++;

        if (fscanf(in, "frame_size.x: %i\n", &temp) != 1)
            break;
        frame_size.x = temp;
        line++;

        if (fscanf(in, "frame_size.y: %i\n", &temp) != 1)
            break;
        frame_size.y = temp;
        line++;

        if (fscanf(in, "frame_size.w: %i\n", &temp) != 1)
            break;
        frame_size.w = temp;
        line++;

        if (fscanf(in, "frame_size.h: %i\n", &temp) != 1)
            break;
        frame_size.h = temp;
        line++;

        if (fscanf(in, "a_script: %99s\n", buffer) != 1)
            break;
        line++;
        a_script = buffer;
        if (a_script == "NONE")
            a_script = "";

        if (fscanf(in, "c_script: %99s\n", buffer) != 1)
            break;
        line++;
        c_script = buffer;
        if (c_script == "NONE")
            c_script = "";

        if (fscanf(in, "collidable: %99s\n", buffer) != 1)
            break;
        if (!strcmp(buffer, "NONE"))
            collidable = NONE;
        else if (!strcmp(buffer, "FULL"))
            collidable = FULL;
        else if (!strcmp(buffer, "MOVE"))
            collidable = MOVE;
        else
            break;
        line++;

        break;
    }

    /*close data file*/
    fclose(in);

    /*check for format errors*/
    if (line < 16) {
        fprintf(stderr, "%s: %i: malformed object file\n", filename, line);
        return false;
    }

    // printf("OBJECT LOADED\n");
    return true;
}

/*********************************************************************
Object::draw()
  draw the object, if the object is visible, active, and on the screen
*********************************************************************/
void Object::draw() {
    SDL_Rect src, dest;
    int wx, wy;
    int diffx, diffy;

    // printf("DEBUG: draw %s\n", name.c_str());

    /*make sure it is visible*/
    if (!visible) {
        printf("DEBUG: %s: %ix%i: not visible\n", name.c_str(), x, y);
        return;
    }

    /*make sure it is active*/
    if (!active) {
        printf("DEBUG: %s: %ix%i: not active\n", name.c_str(), x, y);
        return;
    }

    /*check for image data*/
    if (image == NULL) {
        printf("DEBUG: no image data\n");
        return;
    }

    /*translate from tile cell to world pixel coordinates*/
    wx = x * TILEWIDTH;
    wy = y * TILEHEIGHT;

    diffx = world.viewport.x - world.viewable.x;
    diffy = world.viewport.y - world.viewable.y;

    dest.x = wx + diffx;
    dest.y = wy + diffy;

    dest.x += offset_x;
    dest.y += offset_y;

    /*adjust by movement step offset, if we have steps left*/
    if (step) {
        switch (direction) {
        case UP:
            dest.y += ObjectStepSize[speed] * step;
            break;
        case DOWN:
            dest.y -= ObjectStepSize[speed] * step;
            break;
        case LEFT:
            dest.x += ObjectStepSize[speed] * step;
            break;
        case RIGHT:
            dest.x -= ObjectStepSize[speed] * step;
            break;
        }
        step--;
    }

    /*copy image frame dimensions to a temp rect, to keep SDL from
    changing it*/
    src.x = frame_size.x;
    src.y = frame_size.y;
    src.w = frame_size.w;
    src.h = frame_size.h;

    if (SDL_BlitSurface(image, &src, screen, &dest) == -1) {
        fprintf(stderr, "blit error: %s\n", SDL_GetError());
    }

    if (!step)
        move();

    // printf("DEBUG: draw %s finished\n", name.c_str());
}

/*********************************************************************
Object::up()
  attempts to move the object up
*********************************************************************/
Object *Object::up(bool move) {
    Object *object;

    /*already moving, sorry*/
    if (step)
        return NULL;

    /*set facing direction*/
    direction = UP;

    /*check if x, y coord will be inside of map area*/
    if ((y - 1 < 0 || y >= world.h) || (x < 0 || x >= world.w))
        return NULL;

    /*check if there is a solid tile in the way*/
    if (world.world[y - 1][x] % 2)
        return NULL;

    /*check if there is an object in that slot*/
    object = objects.collide(x, y - 1, move, UP);

    /*if moving into this slot is allowed*/
    if (move) {
        /*if we are moving an object, slow down to that objects speed*/
        if (object != NULL && object->collidable == MOVE) {
            normal_speed = speed;
            speed = object->speed;
        }
        step = ObjectSteps[speed];
        y--;
    }

    /*return the collided object, if any*/
    return object;
}

/*********************************************************************
Object::down()
  attempts to move the object down
*********************************************************************/
Object *Object::down(bool move) {
    Object *object;

    /*already moving, sorry*/
    if (step)
        return NULL;

    /*set facing direction*/
    direction = DOWN;

    /*check if x, y coord will be inside of map area*/
    if ((y < 0 || y + 1 >= world.h) || (x < 0 || x >= world.w))
        return NULL;

    /*check if there is a solid tile in the way*/
    if (world.world[y + 1][x] % 2)
        return NULL;

    /*check if there is an object in that slot*/
    object = objects.collide(x, y + 1, move, DOWN);

    /*if moving into this slot is allowed*/
    if (move) {
        /*if we are moving an object, slow down to that objects speed*/
        if (object != NULL && object->collidable == MOVE) {
            normal_speed = speed;
            speed = object->speed;
        }
        step = ObjectSteps[speed];
        y++;
    }

    /*return the collided object, if any*/
    return object;
}

/*********************************************************************
Object::left()
  attempts to move the object left
*********************************************************************/
Object *Object::left(bool move) {
    Object *object;

    /*already moving, sorry*/
    if (step)
        return NULL;

    /*set facing direction*/
    direction = LEFT;

    /*check if x, y coord will be inside of map area*/
    if ((y < 0 || y >= world.h) || (x - 1 < 0 || x >= world.w))
        return NULL;

    /*check if there is a solid tile in the way*/
    if (world.world[y][x - 1] % 2)
        return NULL;

    /*check if there is an object in that slot*/
    object = objects.collide(x - 1, y, move, LEFT);

    /*if moving into this slot is allowed*/
    if (move) {
        /*if we are moving an object, slow down to that objects speed*/
        if (object != NULL && object->collidable == MOVE) {
            normal_speed = speed;
            speed = object->speed;
        }
        step = ObjectSteps[speed];
        x--;
    }

    /*return the collided object, if any*/
    return object;
}

/*********************************************************************
Object::right()
  attempts to move the object right
*********************************************************************/
Object *Object::right(bool move) {
    Object *object;

    /*already moving, sorry*/
    if (step)
        return NULL;

    /*set facing direction*/
    direction = RIGHT;

    /*check if x, y coord will be inside of map area*/
    if ((y < 0 || y >= world.h) || (x < 0 || x + 1 >= world.w))
        return NULL;

    /*check if there is a solid tile in the way*/
    if (world.world[y][x + 1] % 2)
        return NULL;

    /*check if there is an object in that slot*/
    object = objects.collide(x + 1, y, move, RIGHT);

    /*if moving into this slot is allowed*/
    if (move) {
        /*if we are moving an object, slow down to that objects speed*/
        if (object != NULL && object->collidable == MOVE) {
            normal_speed = speed;
            speed = object->speed;
        }
        step = ObjectSteps[speed];
        x++;
    }

    /*return the collided object, if any*/
    return object;
}

/*********************************************************************
Object::activate()
  tries to activate an object 1 tile in the current facing direction.
  returns the object (if there is one), or NULL for invalid direction
  (STOP) or no object
*********************************************************************/
Object *Object::activate() {
    Object *object;
    int obj_x, obj_y;

    obj_x = x;
    obj_y = y;

    switch (direction) {
    case UP:
        obj_y--;
        break;
    case DOWN:
        obj_y++;
        break;
    case LEFT:
        obj_x--;
        break;
    case RIGHT:
        obj_x++;
        break;
    }

    /*check if x, y coordinates are inside the map area*/
    if ((obj_y < 0 || obj_y >= world.h) || (obj_x < 0 || obj_x >= world.w)) {
        return NULL;
    }

    /*check if there is an object in that slot*/
    object = objects.find(obj_x, obj_y);

    /*return the collision script filename, if any*/
    return object;
}

/*********************************************************************
Object::move()
  move the object along its path
*********************************************************************/
void Object::move() {
    Object *collide_object;
    PathNode *waypoint;
    SDL_Rect old;

    /*take a moment to adjust the movement speed*/
    if (speed != normal_speed)
        speed = normal_speed;

    /*check if we have directional moves queued up, and if so, do the next one*/
    if (move_stack.stack_len) {
        switch (move_stack.pop()) {
        case UP:
            collide_object = up();
            break;
        case DOWN:
            collide_object = down();
            break;
        case LEFT:
            collide_object = left();
            break;
        case RIGHT:
            collide_object = right();
            break;
        }
        if (collide_object != NULL) {
            // printf("WE COLLIDED W/ %s!\n", collide_object->name.c_str());
            /*WARNING:
            using 'this' makes it a const pointer, this may cause horrible problems
            if we try to change things about this object from the script. hopefully
            it won't, since we recast it before we call the script, it still points
            at the same memory location, afterall.
            */
            /*save old clipping rectangle*/
            SDL_GetClipRect(screen, &old);
            /*unset the clipping rectangle, since we are almost certainly being
            triggered from the ObjectList.draw() loop, where we have restricted
            the screen surface's clipping rectangle, and the script may call other
            graphics functions*/
            SDL_SetClipRect(screen, NULL);
            if (!runScript(*((Object *)this), *(collide_object), COLLISION)) {
                printf("SCRIPT EXITED W/ FAILURE STATUS\n");
                return;
            }
            /*restore clipping rectangle*/
            SDL_SetClipRect(screen, &old);
        }
    } else {
        /*get next waypoint*/
        waypoint = path.next();
        /*if we don't have one, we're done*/
        if (waypoint == NULL)
            return;
        /*queue up path to next waypoint*/
        path_find(*(this), waypoint->x, waypoint->y);
    }
}

/*********************************************************************
Object::setSpeed(int s)
  sets the objects speed (the expected speed, that it will try to
  maintain, not its current speed, which is dependent on other things)
*********************************************************************/
void Object::setSpeed(int s) { normal_speed = (Speed)s; }

/*********************************************************************
Speed Object::getSpeed()
  returns the actual (current) speed of the object
*********************************************************************/
Speed Object::getSpeed() { return speed; }

/*********************************************************************
AnimatedObject constructor
  initializes the animated object's data to known values
*********************************************************************/
AnimatedObject::AnimatedObject() {
    int i;

    name = "";
    x = 0;
    y = 0;
    visible = false;
    persistent = false;
    active = false;
    moving = false;
    elapsed = 0;
    zindex = 0;
    offset_x = 0;
    offset_y = 0;
    for (i = 0; i < 5; ++i) {
        image[i] = NULL;
        move_frame_start[i] = 0;
        last_frame[i] = 0;
    }
    frame_size.x = 0;
    frame_size.y = 0;
    frame_size.w = 0;
    frame_size.h = 0;
    a_script = "";
    c_script = "";
    collidable = NONE;
    step = 0;
    speed = WALK;
    normal_speed = speed;
    direction = DOWN;
}

/*********************************************************************
AnimatedObject destructor
  clean up after the animated object
*********************************************************************/
AnimatedObject::~AnimatedObject() {
    int i;

    for (i = 0; i < RIGHT + 1; ++i) {
        if (image[i] != NULL) {
            SDL_FreeSurface(image[i]);
            image[i] = NULL;
        }
    }
}

/*********************************************************************
AnimatedObject::load()
  initializes the animated object from a data file
*********************************************************************/
bool AnimatedObject::load(const char *filename) {
    FILE *in;
    char buffer[100];
    int temp;
    int line;
    int i;

    // printf("LOADING ANIMATED OBJECT: %s\n", filename);
    /*open data file (in text mode) for reading*/
    in = fopen(filename, "rt");
    if (in == NULL) {
        fprintf(stderr, "%s: ", filename);
        perror("error");
        return false;
    }

    line = 1;
    while (true) {
        if (fscanf(in, "name: %99s\n", buffer) != 1)
            break;
        line++;
        name = buffer;

        if (fscanf(in, "visible: %i\n", &temp) != 1)
            break;
        line++;
        visible = temp;

        if (fscanf(in, "persistent: %i\n", &temp) != 1)
            break;
        line++;
        persistent = temp;

        if (fscanf(in, "active: %i\n", &temp) != 1)
            break;
        line++;
        active = temp;

        if (fscanf(in, "zindex: %f\n", &zindex) != 1)
            break;
        line++;

        if (fscanf(in, "offset_x: %i\n", &offset_x) != 1)
            break;
        line++;

        if (fscanf(in, "offset_y: %i\n", &offset_y) != 1)
            break;
        line++;

        if (fscanf(in, "image_down: %99s\n", buffer) != 1)
            break;
        /*try to load image, if any*/
        if (strcmp(buffer, "NONE")) {
            image[DOWN] = IMG_Load(buffer);
            if (image[DOWN] == NULL) {
                fprintf(stderr, "%s: %i: %s\n", filename, line, IMG_GetError());
                break;
            }
        }
        line++;

        if (fscanf(in, "move_frame_start: %i\n", &move_frame_start[DOWN]) != 1)
            break;
        line++;

        if (fscanf(in, "image_up: %99s\n", buffer) != 1)
            break;
        /*try to load image, if any*/
        if (strcmp(buffer, "NONE")) {
            image[UP] = IMG_Load(buffer);
            if (image[UP] == NULL) {
                fprintf(stderr, "%s: %i: %s\n", filename, line, IMG_GetError());
                break;
            }
        }
        line++;

        if (fscanf(in, "move_frame_start: %i\n", &move_frame_start[UP]) != 1)
            break;
        line++;

        if (fscanf(in, "image_left: %99s\n", buffer) != 1)
            break;
        /*try to load image, if any*/
        if (strcmp(buffer, "NONE")) {
            image[LEFT] = IMG_Load(buffer);
            if (image[LEFT] == NULL) {
                fprintf(stderr, "%s: %i: %s\n", filename, line, IMG_GetError());
                break;
            }
        }
        line++;

        if (fscanf(in, "move_frame_start: %i\n", &move_frame_start[LEFT]) != 1)
            break;
        line++;

        if (fscanf(in, "image_right: %99s\n", buffer) != 1)
            break;
        /*try to load image, if any*/
        if (strcmp(buffer, "NONE")) {
            image[RIGHT] = IMG_Load(buffer);
            if (image[RIGHT] == NULL) {
                fprintf(stderr, "%s: %i: %s\n", filename, line, IMG_GetError());
                break;
            }
        }
        line++;

        if (fscanf(in, "move_frame_start: %i\n", &move_frame_start[RIGHT]) != 1)
            break;
        line++;

        if (fscanf(in, "frame_size.x: %i\n", &temp) != 1)
            break;
        frame_size.x = temp;
        line++;

        if (fscanf(in, "frame_size.y: %i\n", &temp) != 1)
            break;
        frame_size.y = temp;
        line++;

        if (fscanf(in, "frame_size.w: %i\n", &temp) != 1)
            break;
        frame_size.w = temp;
        line++;

        if (fscanf(in, "frame_size.h: %i\n", &temp) != 1)
            break;
        frame_size.h = temp;
        line++;

        if (fscanf(in, "a_script: %99s\n", buffer) != 1)
            break;
        line++;
        a_script = buffer;
        if (a_script == "NONE")
            a_script = "";

        if (fscanf(in, "c_script: %99s\n", buffer) != 1)
            break;
        line++;
        c_script = buffer;
        if (c_script == "NONE")
            c_script = "";

        if (fscanf(in, "collidable: %99s\n", buffer) != 1)
            break;
        if (!strcmp(buffer, "NONE"))
            collidable = NONE;
        else if (!strcmp(buffer, "FULL"))
            collidable = FULL;
        else if (!strcmp(buffer, "MOVE"))
            collidable = MOVE;
        else
            break;
        line++;

        break;
    }

    /*close data file*/
    fclose(in);

    /*check for format errors*/
    if (line < 23) {
        fprintf(stderr, "%s: %i: malformed animated object file\n", filename, line);
        return false;
    }

    /*calculate last frame for each image's animation*/
    for (i = 0; i < RIGHT + 1; ++i) {
        if (image[i] == NULL) {
            last_frame[i] = 0;
            continue;
        }
        last_frame[i] = image[i]->w / frame_size.w;
        if (move_frame_start[i] > last_frame[i]) {
            printf("warning: move frame start %i is larger than last frame\n", i);
            move_frame_start[i] = 0;
        }
    }

    return true;
}

/*********************************************************************
AnimatedObject::draw()
  draw the animated object, if the object is visible, active, and on
  the screen, w/ the appropriate frame of animation
*********************************************************************/
void AnimatedObject::draw() {
    SDL_Rect src, dest;
    int wx, wy;
    int diffx, diffy;
    SDL_Surface *img;
    int i;

    // printf("DEBUG: animate %s\n", name.c_str());

    /*make sure it is visible*/
    if (!visible) {
        printf("DEBUG: %s: %ix%i: not visible\n", name.c_str(), x, y);
        return;
    }

    /*make sure it is active*/
    if (!active) {
        printf("DEBUG: %s: %ix%i: not active\n", name.c_str(), x, y);
        return;
    }

    /*translate from tile cell to world pixel coordinates*/
    wx = x * TILEWIDTH;
    wy = y * TILEHEIGHT;

    /*calculate difference between world and screen coordinates*/
    diffx = world.viewport.x - world.viewable.x;
    diffy = world.viewport.y - world.viewable.y;

    /*set drawing destination in terms of screen coordinates*/
    dest.x = wx + diffx;
    dest.y = wy + diffy;

    /*adjust drawing destination by object offsets*/
    dest.x += offset_x;
    dest.y += offset_y;

    update();

    /*adjust by movement step offset, if we have steps left*/
    if (step) {
        switch (direction) {
        case UP:
            dest.y += ObjectStepSize[speed] * step;
            break;
        case DOWN:
            dest.y -= ObjectStepSize[speed] * step;
            break;
        case LEFT:
            dest.x += ObjectStepSize[speed] * step;
            break;
        case RIGHT:
            dest.x -= ObjectStepSize[speed] * step;
            break;
        }
        step--;
    }

    /*copy image frame dimensions to a temp rect, to keep SDL from
    changing it*/
    src.x = frame_size.x;
    src.y = frame_size.y;
    src.w = frame_size.w;
    src.h = frame_size.h;

    /*check for image data*/
    if (image[direction] == NULL) {
        printf("%s: no image data for direction: %i\n", name.c_str(), direction);
        frame = 0;
        img = NULL;
        for (i = 0; i < RIGHT + 1; ++i) {
            if (image[i] == NULL)
                continue;
            img = image[i];
            break;
        }
    } else {
        img = image[direction];
    }

    /*adjust src for current frame of animation*/
    src.x += frame_size.w * frame;
    // printf("FRAME: %i\n", frame);

    if (img != NULL) {
        if (SDL_BlitSurface(img, &src, screen, &dest) == -1) {
            fprintf(stderr, "blit error: %s\n", SDL_GetError());
        }
    }

    if (!step)
        move();

    // printf("DEBUG: animate %s finished\n", name.c_str());
}

/*********************************************************************
AnimatedObject::update()
  updates which frame of animation the object is on
*********************************************************************/
#define ANIM_INTERVAL 250
#define ANIM_MOVE_INTERVAL 100
void AnimatedObject::update() {

    elapsed += msecs_per_frame;

    // printf("HELO, %i: %i\n", frame, elapsed);
    if (step > 0) {
        // printf("STEP\n");
        if (moving) {
            // printf("MOVING\n");
            if (elapsed >= ANIM_MOVE_INTERVAL) {
                elapsed = 0;
                frame++;
                if (frame >= last_frame[direction])
                    frame = move_frame_start[direction];
            }
        } else {
            // printf("JUST STARTED MOVING\n");
            moving = true;
            frame = move_frame_start[direction];
            if (frame >= last_frame[direction]) {
                frame = 0;
            }
        }
    } else {
        // printf("NOSTEP\n");
        if (moving || elapsed >= ANIM_INTERVAL) {
            elapsed = 0;
            frame++;
            if (frame >= move_frame_start[direction]) {
                frame = 0;
            }
        }
        moving = false;
    }
    /*loop animation as necessary*/
    if (frame >= last_frame[direction])
        frame = 0;
}

/*********************************************************************
ObjectList destructor
  just traverse the list, freeing all nodes/objects
*********************************************************************/
ObjectList::~ObjectList() {
    ObjectListNode *search, *temp;

    search = head;
    while (search != NULL) {
        temp = search;
        search = search->next;
        if (temp->object != NULL) {
            if (temp->animated)
                delete (AnimatedObject *)temp->object;
            else
                delete temp->object;
            temp->object = NULL;
        }
        delete temp;
    }
    head = NULL;
}

/*********************************************************************
ObjectList add()
  attempts to load an object definition from a file (filename)
  and then adds the loaded node (if any) to the list (sorted by the
  zindex) with the current (global) areaname as the area.
  returns a pointer to the added node (if any) or NULL on failure
*********************************************************************/
Object *ObjectList::add(const char *filename, int y) {
    ObjectListNode *node, *search;
    const char *string;

    /*create new node*/
    node = new ObjectListNode;

    /*figure out if we have an animated object, based on the filename*/
    string = strrchr(filename, '.');
    if (string != NULL) {
        /*check if the filename ends in ".anim"*/
        if (!strcmp(string, ".anim")) {
            node->animated = true;
        } else if (strcmp(string, ".object")) {
            fprintf(stderr, "warning: %s: unknown extension: %s\n", filename, string);
        }
    }

    /*create new object/animated object*/
    if (node->animated) {
        node->object = new AnimatedObject;
        /*attempt to load AnimatedObject definition from file*/
        if (!((AnimatedObject *)node->object)->load(filename)) {
            /*unable to load object/error in file, delete the new node and object*/
            delete (AnimatedObject *)node->object;
            node->object = NULL;
            delete node;
            /*abort add*/
            return NULL;
        }
    } else {
        node->object = new Object;
        /*attempt to load Object definition from file*/
        if (!node->object->load(filename)) {
            /*unable to load object/error in file, delete the new node and object*/
            delete node->object;
            node->object = NULL;
            delete node;
            /*abort add*/
            return NULL;
        }
    }

    /*mark which area this item belongs to*/
    node->area = world.name;

    /*if this is a persistent object, check for previous instance*/
    if (node->object->persistent) {
        search = head;
        while (search != NULL) {
            /*if this name already exists in this area (and is persistent)*/
            if ((search->object->name == node->object->name) && (search->area == node->area)) {
                printf("DEBUG: %s already exists in %s\n", search->object->name.c_str(), search->area.c_str());
                if (node->animated)
                    delete (AnimatedObject *)node->object;
                else
                    delete node->object;
                node->object = NULL;
                delete node;           /*remove the new copy*/
                return search->object; /*return the old object*/
            }
            search = search->next;
        }
    }

    /*set key value, should keep adding it to a displaylist later fast,
    as long as it doesn't move around (y axis) much*/
    node->layer = (float)(node->object->zindex) + (float)y / 100.0;

    /*not already in list, insert it*/
    search = head;
    /*not an empty list?*/
    if (search != NULL) {
        /*find node in front of insertion position*/
        while ((search->next != NULL) && (search->next->layer > node->layer)) {
            search = search->next;
        }
    }

    /*insert at front?*/
    if ((head == NULL) || (search == head && search->layer <= node->layer)) {
        // printf("INSERT: at front\n");
        node->next = search;
        head = node;
    } else
        /*insert at end?*/
        if (search->next == NULL) {
        // printf("INSERT: at end\n");
        search->next = node;
    } else
    /*insert in middle*/
    {
        // printf("INSERT: in middle\n");
        node->next = search->next;
        search->next = node;
    }

    /*return the added node's object*/
    return node->object;
}

/*********************************************************************
ObjectList draw()
  just traverse the list, drawing active objects
*********************************************************************/
void ObjectList::draw() {
    ObjectListNode *search;

    search = head;
    while (search != NULL) {
        if (search->object->active) {
            if (search->animated)
                ((AnimatedObject *)search->object)->draw();
            else
                search->object->draw();
        }
        search = search->next;
    }
}

/*********************************************************************
ObjectList collide()
  check if there exists an active/collidable object at the given x, y
  coordinates in the current area
  SPAGHETTI CODE BELOW
*********************************************************************/
Object *ObjectList::collide(int x, int y, bool &move, Direction dir) {
    ObjectListNode *search, *temp;
    Object *object;
    int oldx, oldy;

    object = NULL;
    temp = NULL;
    search = head;
    while (search != NULL) {
        /*if active, at this x,y coordinate, and in this area*/
        if (search->object->active && search->object->x == x && search->object->y == y && search->area == world.name) {
            switch (search->object->collidable) {
            /*no collision problems*/
            case NONE:
                if (temp == NULL) {
                    temp = search;
                }
                search = search->next;
                continue;
            /*static object*/
            case FULL:
                if (move == false) {
                    return NULL;
                }
                move = false;
                break;
            /*collided w/ moveable object*/
            case MOVE:
                /*whether or not we are allowed to move another object*/
                if (move) {
                    move = false;
                    oldx = search->object->x;
                    oldy = search->object->y;
                    switch (dir) {
                    case UP:
                        search->object->up(move);
                        break;
                    case DOWN:
                        search->object->down(move);
                        break;
                    case LEFT:
                        search->object->left(move);
                        break;
                    case RIGHT:
                        search->object->right(move);
                        break;
                    }
                    if (oldx != search->object->x || oldy != search->object->y)
                        move = true;
                } else
                    return NULL;
            default:
                break;
            }
            return search->object;
        }
        search = search->next;
    }
    /*if we hit a non-collidable object, check if it had a script*/
    if (move && temp != NULL) {
        return temp->object;
    }
    move = true;
    return NULL;
}

/*********************************************************************
ObjectList find()
  check if there exists an active/collidable object at the given x, y
  coordinates in the current area, and returns the one w/ the
  highest zindex (topmost visible object)
*********************************************************************/
Object *ObjectList::find(int x, int y) {
    ObjectListNode *search;
    Object *temp;

    temp = NULL;
    search = head;
    while (search != NULL) {
        /*if active, at this x,y coordinate, and in this area*/
        if (search->object->active && search->object->x == x && search->object->y == y && search->area == world.name) {
            temp = search->object;
            break;
        }
        search = search->next;
    }

    return temp;
}

/*********************************************************************
ObjectList activate()
  just traverse the list, setting the active flag according to the
  current (global) areaname
*********************************************************************/
void ObjectList::activate() {
    ObjectListNode *search;

    // printf("DEBUG: activate: %s\n", world.name.c_str());

    search = head;
    while (search != NULL) {
        // printf("Object: %s - %s\n", search->object.name.c_str(),
        //       search->area.c_str());
        if (search->area == world.name)
            search->object->active = true;
        else
            search->object->active = false;
        search = search->next;
    }
}

/*********************************************************************
ObjectList flush()
  flush non-persistent nodes
*********************************************************************/
void ObjectList::flush() {
    ObjectListNode *search, *prev, *temp;

    search = head;
    prev = NULL;
    while (search != NULL) {
        /*it's a keeper*/
        if (search->object->persistent) {
            /*mark as inactive*/
            search->object->active = false;
            /*if the isn't the first node, set the previous node's next pointer*/
            if (prev != NULL)
                prev->next = search;
            /*else set the head pointer to point here*/
            else
                head = search;
            /*set this as the previous preserved node*/
            prev = search;
            search = search->next;
            continue;
        }
        /*delete this node*/
        temp = search;
        search = search->next;
        if (temp->object != NULL) {
            if (temp->animated)
                delete (AnimatedObject *)temp->object;
            else
                delete temp->object;
            temp->object = NULL;
        }
        delete temp;
    }
    /*make sure list is NULL terminated*/
    if (prev != NULL)
        prev->next = NULL;
    else
        head = NULL;
}

/*********************************************************************
ObjectList clear()
  delete all nodes
*********************************************************************/
void ObjectList::clear() {
    ObjectListNode *search, *temp;

    search = head;
    while (search != NULL) {
        temp = search;
        search = search->next;
        if (temp->object != NULL) {
            if (temp->animated)
                delete (AnimatedObject *)temp->object;
            else
                delete temp->object;
            temp->object = NULL;
        }
        delete temp;
    }
    head = NULL;
}

/*********************************************************************
ObjectList debug()
  just traverse the list, printing debugging info
*********************************************************************/
void ObjectList::debug() {
    ObjectListNode *search;
    int nodes;

    printf("ObjectList: DEBUG\n");

    search = head;
    nodes = 0;
    while (search != NULL) {
        printf("Object: %s\n", search->object->name.c_str());
        search = search->next;
        nodes++;
    }

    printf("ObjectList: %i nodes total\n\n", nodes);
}

/*********************************************************************
ObjectLayerList destructor
  just traverse the list, freeing all nodes
*********************************************************************/
ObjectLayerList::~ObjectLayerList() {
    ObjectListNode *search, *temp;

    search = head;
    while (search != NULL) {
        temp = search;
        search = search->next;
        delete temp;
    }
    head = NULL;
}

/*********************************************************************
ObjectLayerList add()
  adds the object to the layer list (sorted by layer)
*********************************************************************/
void ObjectLayerList::add(Object &object) {
    ObjectListNode *node, *search;

    // printf("ADD OBJECT\n");
    // printf("%s\n", object.name.c_str());
    /*don't bother w/ inactive or invisible objects*/
    if (!object.active || !object.visible)
        return;

    /*create new node*/
    node = new ObjectListNode;
    /*set node to point at this object*/
    node->object = &object;
    /*set node layer key*/
    node->layer = (float)object.zindex + ((float)object.y / 100.0);

    search = head;
    /*not an empty list?*/
    if (search != NULL) {
        /*find node in front of insertion position*/
        while ((search->next != NULL) && (search->next->layer < node->layer)) {
            search = search->next;
        }
    }

    /*insert at front?*/
    if ((head == NULL) || (search == head && search->layer >= node->layer)) {
        // printf("INSERT: at front\n");
        node->next = search;
        head = node;
        return;
    }
    /*insert at end?*/
    if (search->next == NULL) {
        // printf("INSERT: at end\n");
        search->next = node;
        return;
    }
    /*insert in middle*/
    // printf("INSERT: in middle\n");
    node->next = search->next;
    search->next = node;
}

/*********************************************************************
ObjectLayerList add()
  adds the animated object to the layer list (sorted by layer)
*********************************************************************/
void ObjectLayerList::add(AnimatedObject &object) {
    ObjectListNode *node, *search;

    // printf("ADD ANIMATED OBJECT\n");
    // printf("%s\n", object.name.c_str());

    /*don't bother w/ inactive or invisible objects*/
    if (!object.active || !object.visible)
        return;

    /*create new node*/
    node = new ObjectListNode;
    /*set node to point at this object*/
    node->object = &object;
    /*set node layer key*/
    node->layer = (float)object.zindex + ((float)object.y / 100.0);
    node->animated = true;

    search = head;
    /*not an empty list?*/
    if (search != NULL) {
        /*find node in front of insertion position*/
        while ((search->next != NULL) && (search->next->layer < node->layer)) {
            search = search->next;
        }
    }

    /*insert at front?*/
    if ((head == NULL) || (search == head && search->layer >= node->layer)) {
        // printf("INSERT: at front\n");
        node->next = search;
        head = node;
        return;
    }
    /*insert at end?*/
    if (search->next == NULL) {
        // printf("INSERT: at end\n");
        search->next = node;
        return;
    }
    /*insert in middle*/
    // printf("INSERT: in middle\n");
    node->next = search->next;
    search->next = node;
}

/*********************************************************************
ObjectLayerList add()
  adds all the objects in the objectlist to the objectlayerlist
*********************************************************************/
void ObjectLayerList::add(ObjectList &list) {
    ObjectListNode *search;

    search = list.head;
    while (search != NULL) {
        if (search->animated)
            add(*((AnimatedObject *)search->object));
        else
            add(*(search->object));
        search = search->next;
    }
}

/*********************************************************************
ObjectLayerList draw()
  just traverse the list, drawing objects
*********************************************************************/
void ObjectLayerList::draw() {
    ObjectListNode *search, *temp;
    SDL_Rect old;

    /*save old clipping rectangle, for future safety*/
    SDL_GetClipRect(screen, &old);

    /*set clipping rectangle to viewport area*/
    SDL_SetClipRect(screen, &world.viewport);

    search = head;
    while (search != NULL) {
        temp = search;
        if (search->animated) {
            ((AnimatedObject *)search->object)->draw();
        } else {
            search->object->draw();
        }

        search = search->next;
        delete temp;
    }
    head = NULL;

    /*restore clipping rectangle*/
    SDL_SetClipRect(screen, &old);
}

/*EOF*/
