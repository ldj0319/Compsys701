v = VideoReader('foreman_qcif.mp4');
max_frames = 300;
for n = 1:max_frames
    video = (read(v,n));
end

createHeaderFile(v, 300);

