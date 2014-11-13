function output_demo_im
%% function to output an image with the "fovea" where you click.

% this is super hacky. Clicking a mouse button changes the blur position,
% and pressing a key saves an image.
% tom wallis wrote it.

fn = 'distorted_letters.png';
halfres = 2.3;  % set to default for humans (2.3)
maparc = 48;  % half of this is the image's horizontal size in degrees.

% Read in the file
fprintf('Reading %s...\n',fn);
img=imread(fn);
rows=size(img,1);
cols=size(img,2);

% if needed, convert grey image to rgb:
if size(img, 3) == 1
    img=repmat(img,[1 1 3]);  % 
end

% Break into separate color planes
red=squeeze(img(:,:,1));
green=squeeze(img(:,:,2));
blue=squeeze(img(:,:,3));

% Initialize the library
svisinit

% Create a resmap
fprintf('Creating resolution map...\n');
resmap=svisresmap(rows*2,cols*2,'halfres',halfres,'maparc',maparc);

% Create 3 codecs for r, g, and b
fprintf('Creating codecs...\n');
c1=sviscodec(red);
c2=sviscodec(green);
c3=sviscodec(blue);

% The masks get created when you set the map
fprintf('Creating blending masks...\n');
svissetresmap(c1,resmap)
svissetresmap(c2,resmap)
svissetresmap(c3,resmap)

% Setup the figure window
hfig=setup_figure(rows,cols);
global window_open
window_open=true;

% Variables for displaying frame rate
tic;
frames=0;

% Start the encoding loop
fprintf('Processing a %d X %d pixel image...\n',cols,rows);
fprintf('Press ESC to exit...\n');

while window_open

    % Get the cursor
    pos=get(gca,'currentpoint');
    row=pos(3);
    col=pos(1);

    % Encode
    i1=svisencode(c1,row,col);
    i2=svisencode(c2,row,col);
    i3=svisencode(c3,row,col);

    % Put them back together
    rgb=cat(3,i1,i2,i3);

    % Display it
    %image(rgb)
    set(hfig,'cdata',rgb);
    set(gca,'tickdir','out');
    drawnow

    w = waitforbuttonpress;
    if w == 0  
        % mouse click, continue updating frames...
    else
        % key press, save image...
       imwrite(rgb, [fn, '_out.jpg']) 
       window_open=false;
    end 
    
    frames=frames+1;

    % Show frame rate
    if toc>5
        fprintf('%.1f Hz\n',frames/toc);
        frames=0;
        tic;
    end
end

% Free resources
svisrelease