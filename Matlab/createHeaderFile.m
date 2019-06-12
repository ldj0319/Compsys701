function createHeaderFile(videoReader, numFrames)
    
    string = strcat('#define NUM_FRAMES ', numFrames, '\nuint8_t frames[NUM_FRAMES][IMAGE_SIZE] = {\n\n');
    
    for frameNum = 1:numFrames+1
        frame = (read(videoReader,frameNum));
        string = strcat(string, createCArrayString(frame), ',', newline, newline);
    end
end