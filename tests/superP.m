function superP
%A = imread('D:\GrainFinder\tests\1000and100\900_3_chas\8\x1000 centr.jpg'); k = 800
A = imread('cutted.jpg'); k = 30;
figure;

[L, N] = superpixels(A, k,'Method', 'slic', 'Compactness', 0.1, 'NumIterations', 100);
subplot(121);
imshow(A);
subplot(122);
BW = boundarymask(L);
imshow(imoverlay(A,BW,'cyan'),'InitialMagnification', 67)
