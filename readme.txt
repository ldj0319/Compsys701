To run the systemC model, navigate to this folder in modelsim, create the work library, and open 'modelsim script.txt'. You can then copy the contents of this file into the modelsim console.
If all goes well, you should see the last line of the modelsim console say 'starting file write...', which means that the output has been written to files, one for each channel.
This frame can be viewed by running the Matlab script 'readRGBOutput.m'.
The 'readRGBInput.m' file opens the frame before the system processes it, in case you want to compare.

To generate the next frame, type 'run 33 ms' in the modelsim console. Repeat as many times as necessary.
You can then load this new frame using the same method as above.
Unfortunately, the current model overwrites each frame with the last, so if you want to view multiple frames in succession, you must run 33 ms, open the file using matlab, then run 33 ms again, open in matlab again, etc...

