R = fread(fopen("Rin.bin"));
G = fread(fopen("Gin.bin"));
B = fread(fopen("Bin.bin"));

R = transpose(reshape(R, [176,144]));
G = transpose(reshape(G, [176,144]));
B = transpose(reshape(B, [176,144]));

imshow(RGB/255)

Rorig = fread(fopen("thereAndBackR.bin"));
Gorig = fread(fopen("thereAndBackG.bin"));
Borig = fread(fopen("thereAndBackB.bin"));

Rorig = transpose(reshape(Rorig, [176,144]));
Gorig = transpose(reshape(Gorig, [176,144]));
Borig = transpose(reshape(Borig, [176,144]));


mean(Rorig(:) - R(:))
mean(Gorig(:) - G(:))
mean(Borig(:) - B(:))
imshow(RGB/255)