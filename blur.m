function blur(fn,e2,e,ppd)
% Blur an image
%
% fn - filename of image
% e2 - halfres in degrees
% e - eccentricity in degrees
% ppd - pixels per degree
%
% jsp Mon Nov 26 17:21:54 CST 2007

% Read it
x=double(imread(fn));
show(x,1,'Original Image')

% Mirror the borders
x=mirror_tile(x);
show(x,2,'Padded Image');

% Get the distance from the center in pixels
m=size(x,1);
n=size(x,2);
[dx,dy]=meshgrid(-(n-1)/2:(n-1)/2,-(m-1)/2:(m-1)/2);
dx=dx/n;
dy=dy/m;
d=sqrt(dx.^2+dy.^2);

% Convert everything to pixels
e=e*ppd;
e2=e2*ppd;
alpha=0.1*ppd;

% Shift in Fourier space -- this is what ifft2 expects.
Y=fftshift(exp(-alpha*d*(e2+e)/e2));
% Shift to center the filter image
y=fftshift(ifft2(Y));
show(abs(untile(y)),3,'Filter Kernel');

% Convert image to Fourier domain
X=fft2(x);
% Do the filtering
Z=X.*Y;
% Convert back to space domain
z=ifft2(Z);
show(untile(real(z)),4,sprintf('e=%f degrees',e/ppd));

function show(x,n,s)
figure(n)
imagesc(x)
colormap(gray);
daspect([1 1 1]);
title(s)

function x=mirror_tile(x)
x=repmat(x,3);
x(1:end/3,:)=flipud(x(1:end/3,:));
x(2*end/3+1:end,:)=flipud(x(2*end/3+1:end,:));
x(:,1:end/3)=fliplr(x(:,1:end/3));
x(:,2*end/3+1:end)=fliplr(x(:,2*end/3+1:end));

function x=untile(x)
x=x(end/3+1:2*end/3,end/3+1:2*end/3);
