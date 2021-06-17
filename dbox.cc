#include <stdio.h>
#include "dbox.hh"
#include "font.hh"
#include "button.hh"
#include "sound.hh"
#include "video.hh"


extern SDL_Surface *screen;
SDL_Rect dbox_rect;


int dboxChoice(const char *title, Choice choices[DBOX_CHOICES])
{
  SDL_Event event;
  int i, num_choices;
  char buffer[DBOX_TEXTMAX];
  int prev, curr, selected;
  int x, y;

  dboxDraw();

  x = dbox_rect.x + DBOX_TEXTLEFT;
  y = dbox_rect.y + DBOX_TEXTTOP;
  prev = curr = 0;
  selected = -1;

  /*truncate menu caption/title as necessary*/
  strncpy(buffer, title, DBOX_TEXTMAX);
  buffer[DBOX_TEXTMAX - 1] = '\0';

  /*draw menu caption/title*/
  writeFont(&dboxFont, screen, x, y, buffer);

  /*draw active choices*/
  for(i=0; i<DBOX_CHOICES; ++i)
  {
    if(choices[i].active == false) break;
    /*truncate choice text as needed*/
    strncpy(buffer, choices[i].text.c_str(), DBOX_TEXTMAX);
    buffer[DBOX_TEXTMAX - 1] = '\0';
    choices[i].text = buffer;
    /*highlight first choice*/
    if(i == 0)
    {
    writeFont(&dboxHighlightFont, screen, x, y + (dboxFont.h * (i+1)),
              choices[i].text.c_str());
    }
    else
    {
    writeFont(&dboxFont, screen, x, y + (dboxFont.h * (i+1)),
              choices[i].text.c_str());
    }
    /*register button geometry for each choice*/
    choices[i].button.top = y + (dboxFont.h * (i+1)),
    choices[i].button.left = x;
    choices[i].button.bottom = choices[i].button.top + dboxFont.h;
    choices[i].button.right = choices[i].button.left +
                              (dboxFont.w * strlen(buffer));
  }

  num_choices = i;
  updateDisplay();
  /*clear event queue before we select a choice*/
  flushEvents();

  while(SDL_WaitEvent(&event))
  {
    /*respect quit events*/
    if(event.type == SDL_QUIT) exit(1);

    /*mouse click events*/
    if(event.type == SDL_MOUSEBUTTONUP)
    {
      /*check if they clicked an actual option/choice*/
      for(i=0; i<num_choices; ++i)
      {
        if(clicked(&event.button, choices[i].button))
        {
          selected = i;
          break;
        }
      }
    }

    /*mouse move events*/
    if(event.type == SDL_MOUSEMOTION)
    {
      for(i=0; i<num_choices; ++i)
      {
        if(mouseOver(&event.button, choices[i].button))
        {
          /*don't update the menu if it doesn't need it*/
          if(i != prev) curr = i;
          break;
        }
      }
    }

    /*keypress events*/
    if(event.type == SDL_KEYUP)
    {
      switch(event.key.keysym.sym)
      {
        case SDLK_UP:
          if(curr <= 0) curr = num_choices-1;
          else curr--;
          break;
        case SDLK_DOWN:
          if(curr >= num_choices-1) curr = 0;
          else curr++;
          break;
        case SDLK_RETURN:
        case SDLK_SPACE:
        case SDLK_KP_ENTER:
          selected = curr;
          break;
        default:
          checkToggles(event.key.keysym);
          break;
      }
    }

    /*check for menu change*/
    if(curr != prev)
    {
      /*play menu change sound*/
      playSample(MENUBLIP);
      /*de-highlight old menu item*/
      writeFont(&dboxFont, screen, x, y+((prev+1)*dboxFont.h),
                choices[prev].text.c_str());
      prev = curr;
      /*highlight new menu item*/
      writeFont(&dboxHighlightFont, screen, x, y+((curr+1)*dboxFont.h),
                choices[curr].text.c_str());
      /*update display*/
      updateDisplay();
      }
    /*stay here until they choose something*/
    if(selected != -1) break;
  }

  /*play menu selection sound*/
  playSample(MENUBLIP);
  dboxErase();
  return choices[selected].value;
}


void dboxText()
{
  int i;

  dboxDraw();

  for(i=0; i<DBOX_MAXLINES; ++i)
  {
    writeFont(&dboxFont, screen,
              dbox_rect.x + DBOX_TEXTLEFT,
              dbox_rect.y + DBOX_TEXTTOP + (dboxFont.h * i),
              dbox_lines[i]);
  }

  updateDisplay();
  delay((unsigned int)-1, true);

  dboxErase();
}


void dboxDraw()
{
  char title[DBOX_TITLEMAX];
  SDL_Surface *dbox_img;
  SDL_Rect src, dest;
  VAR_param *var;

  /*if sounds are on, play the dialog box sound (if any)*/
  if(config.sounds)
  {
    if(dialog_vars.eval("DBOXSND"))
    {
      var = dialog_vars.get("DBOXSND");
      if(var->type != VAR_STRING) dbox_snd = NULL;
      else
      {
        dbox_snd = playFile(var->data.string_data->c_str(),
                            dialog_vars.toInt("DBOXSNDLOOPS"));
      }
    }
  }

  /*unset the sound variables*/
  dialog_vars.set("DBOXSND", "int", NULL);
  dialog_vars.set("DBOXSNDLOOPS", "int", NULL);

  /*load dialogbox pic, if any is set*/
  var = dialog_vars.get("DBOXIMG");
  if(var == NULL || var->type != VAR_STRING) dbox_img = NULL;
  else
  {
    dbox_img = IMG_Load(var->data.string_data->c_str());
    if(dbox_img == NULL)
    {
      fprintf(stderr, "error opening image '%s': %s",
              var->data.string_data->c_str(), SDL_GetError());
    }
  }

  /*set dialogbox position/rect*/
  dbox_rect.x = DBOX_LEFT;
  if(dialog_vars.eval("DBOXTOP"))
    dbox_rect.y = DBOX_TOP;
  else
    dbox_rect.y = screen->h - DBOX_TOP - dbox->h;
  dbox_rect.w = dbox->w;
  dbox_rect.h = dbox->h;

  /*save previous screen data*/
  SDL_BlitSurface(screen, &dbox_rect, dbox_back, NULL);

  /*draw the dialog box*/
  SDL_BlitSurface(dbox, NULL, screen, &dbox_rect);

  /*draw the dialog box pic, if any*/
  if(dbox_img != NULL)
  {
    src.x = 0;
    src.y = 0;
    src.w = 95;
    src.h = 145;
    dest.x = dbox_rect.x + DBOX_IMGLEFT;
    dest.y = dbox_rect.y + DBOX_IMGTOP;
    SDL_BlitSurface(dbox_img, &src, screen, &dest);
    SDL_FreeSurface(dbox_img);
  }

  /*get dialog set user title*/
  if(dialog_vars.toString("DBOXTITLE", title, DBOX_TITLEMAX) == NULL)
  {
    /*none set, use default*/
    strncpy(title, DBOX_TITLEDEFAULT, DBOX_TITLEMAX-1);
    /*make sure it is null terminated*/
    title[DBOX_TITLEMAX-1] = '\0';
  }

  /*draw title/caption*/
  writeFont(&dboxFont, screen, dbox_rect.x+DBOX_TITLELEFT,
            dbox_rect.y+DBOX_TITLETOP, title);
}


void dboxErase()
{
  /*take this time to free the dbox_snd chunk*/
  if(dbox_snd != NULL)
  {
    Mix_FreeChunk(dbox_snd);
    dbox_snd = NULL;
  }

  /*restore the screen data*/
  SDL_BlitSurface(dbox_back, NULL, screen, &dbox_rect);
}

/*EOF*/
