#!/bin/bash

for FILE in *.png
do
    echo "$FILE";
    if pngfix "$FILE" | grep "TFB";
    then
        echo "Fixing $FILE IDAT TFB";
        # Fix the file and copy it to $FILE_idat_fixed
        pngfix -o --strip=transform --suffix=_idat_fixed "$FILE"
        # Rename the fixed file to the old filename
        mv "$FILE"_idat_fixed "$FILE"
    fi
done;
