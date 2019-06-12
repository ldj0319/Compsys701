Rid = fopen("thereAndBackR.bin");
Gid = fopen("thereAndBackG.bin");
Bid = fopen("thereAndBackB.bin");

R = fread(Rid);
G = fread(Gid);
B = fread(Bid);

fclose(Rid);
fclose(Gid);
fclose(Bid);

R = transpose(reshape(R, [176,144]));
G = transpose(reshape(G, [176,144]));
B = transpose(reshape(B, [176,144]));

RGB(:,:,1) = R;
RGB(:,:,2) = G;
RGB(:,:,3) = B;

imshow(RGB/255)