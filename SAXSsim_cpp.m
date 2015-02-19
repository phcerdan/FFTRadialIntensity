% SAS simulator, read in an image and output a q vs I plot
function [pixelDist,Idat] = SAXSsim(image,nm_per_pixel)

sum2D = @(a) sum(reshape(a,1,[]));             % sum elements in 2D matrix
u_FFT = @(N) single((-1/2):(1/N):(1/2 - 1/N)); % centered unity
% ==== Inputs
units = 'nm';
calConst = 1.0/nm_per_pixel; % calibration constant. in units of pixels per micron
% If 1pixel = 0.86 nm ->calConst = 1.0/0.86
% If 1pixel = 1.72nm ->calConst = 1.0/1.72
% 
% == end of inputs

% coordinates and resolutions
dx = 1/calConst; % spatial resolution [units]
Nx = numel(image(1,:)); %Size of input image in x dimension
Ny = numel(image(:,1)); %Size of input image in y dimension
x = ( Nx*u_FFT(Nx) )*dx; % 1D x spatial coordinate [units]
y = ( Ny*u_FFT(Ny) )*dx; % 1D y spatial coordinate [units]
dfx = 1/Nx/dx;  % resolution in frequency plane x [1/units]
dfy = 1/Ny/dx;   % resolution in frequency plane y [1/units]
fx = ( Nx*u_FFT(Nx) )*dfx;      % 1D spatial freq. coordinate [1/units]
fy = ( Ny*u_FFT(Ny) )*dfy;      % 1D spatial freq. coordinate [1/units]
dx2 = 1/Nx/dfx;               % inverse-transformed x spatial resolution [units]     
dy2 = 1/Ny/dfy;               % inverse-transformed y spatial resolution [units]     
x2 = (Nx*u_FFT(Nx) )*dx2;     % inverse-transformed spatial coordinate [units]
y2 = (Ny*u_FFT(Ny) )*dy2;     % inverse-transformed spatial coordinate [units]

f_Nyq = 1/2/dx;             % max observable (+/-) spatial freq. [1/units]

% Perfprm FFT
g = double(image);
G = dx^2*fftshift(fft2(g)); % Perform Fourier Transform: spatial->freq
g2 = (Nx*dfx)*(Ny*dfy)*ifft2(G);     % Perform inverse transform: freq->spatial
 
 %check FFT Parseval sums:  int(|g|^2*dx*dy) =should= int(|G|^2*dfx*dfy)
 
spatialSum = sum2D(abs(g).^2)*dx*dx;    % g units:  [Amplitude/sqrt(m)]
freqSum    = sum2D(abs(G).^2)*dfx*dfy;    % G units:  [Amplitude*sqrt(m)]
spatialSum2= sum2D(abs(g2).^2)*dx2*dy2;

ifig=0; %figure counter
 % Plots
    ifig=ifig+1;
    figure('name',strcat(inputname(1),' |g|^2: spatial domain')); imagesc(x,y,abs(g).^2);axis('square');
    xlabel(strcat('x [', units, ']' ));ylabel(strcat('y [', units, ']' )); colorbar;
    title(sprintf('%s = %1.4f,  N = %i,  spatialSum = %1.5f','\Deltax',dx,Nx,spatialSum));
    
    ifig=ifig+1;
    figure('name',strcat(inputname(1),' |G|^2: freq domain'));   imagesc(fx,fy,log2(G.*conj(G)));axis('square');
    xlabel(strcat('f_x [1/',units,']' )); ylabel(strcat('f_y [1/',units,']' )); colorbar
    title(sprintf('f_N_y_q = %1.2f,  %s = %1.5f,  freqSum = %1.5f',f_Nyq,'\Deltaf',dfx,freqSum));
    
%     ifig=ifig+1;
%     figure('name',strcat(inputname(1), ' |g2|^2: spatial domain'));imagesc(x2,y2,abs(g2).^2);axis('square');
%     xlabel('x2 [mm]');ylabel('y2 [mm]'); colorbar
%     title(sprintf('%s = %1.4f,  N = %i,  spatialSum2 = %1.5f','\Deltax2',dx2,Nx,spatialSum2));
   
    fprintf('%s\n',repmat('=',1,70));
    %fprintf('Airy radius (frequency):       %1.3f 1/units\n',2.44/2/D);
    %fprintf('Samples w/in Airy diameter:    %1.2f\n',2.44/D/df);
    %fprintf('Samples w/in spatial disk:     %1.2f\n',D/dx);

    c1 = numel(G(1,:))/2;
    c2 = numel(G(:,1))/2;
    
    xpos = 1:numel(G(1,:));
    ypos = 1:numel(G(:,1));
        %pos(number) = sqrt(abs(xpos-numel(circle(1,:))/2)^2+abs(ypos-numel(circle(:,1))/2)^2);
        %pos(number) = sqrt(abs(xpos-250)^2+abs(ypos-250)^2);
        %Position of the pixel from the center.
        matx = repmat(xpos,numel(xpos),1);
        maty = repmat(ypos',1,numel(ypos));
        pos2 = sqrt((matx-c1).^2 + (maty-c2).^2);
        %Data is the value of the pixel.
        data_mat = G.*conj(G);
   pos = reshape(pos2,1, numel(pos2));
   data = reshape(data_mat,1,numel(data_mat));
%     for xpos = 1:numel(G(1,:))
%     for ypos = 1:numel(G(:,1))
%         %pos(number) = sqrt(abs(xpos-numel(circle(1,:))/2)^2+abs(ypos-numel(circle(:,1))/2)^2);
%         %pos(number) = sqrt(abs(xpos-250)^2+abs(ypos-250)^2);
%         %Position of the pixel from the center.
%         pos(number) = sqrt(abs(xpos-numel(G(1,:))/2)^2+abs(ypos-numel(G(:,1))/2)^2);
%         %Data is the value of the pixel.
%         data(number) = G(ypos,xpos)*conj(G(ypos,xpos));
%         number = number + 1;
%     end
%     end
    

%idx is index. Index associated to number of pixels.    
IDX = uint32(1:numel(pos));
number =1;
bin = 1; %bin size for pixels
clear ind
for jj = 0:bin:max(pos)-bin
    ind.ind{number} = IDX(pos > jj & pos <= jj+bin);
    number=number+1;
end
Idat = zeros(1,numel(ind.ind));
parfor kk = 1:numel(ind.ind)
    Idat(kk) = mean(data(ind.ind{kk}));
end
pixelDist = 0:bin*dfx:bin*numel(Idat)*dfx; %distance from image centre in pix in units
 ifig=ifig+1;
 %figure('name','simulated scattering output');
 figure('name',inputname(1)); %the name of the variable of the argument function.
 loglog(pixelDist(1:numel(Idat)),Idat,'r^');
 xlabel(strcat('q [1/',units,'] (nm-per-pixel= ',num2str(nm_per_pixel),' )')); ylabel('I [arb]');
 %title(sprintf('f_N_y_q = %1.2f,  %s = %1.5f,  freqSum = %1.5f',f_Nyq,'\Deltaf',dfx,freqSum));


%angle = atan(pixelDist.*4.49e-4); % angle is atan(pixelDist/smple to detector dist)
%q = 4*pi.*sin(angle./2)./0.15e-9; % calculate q for copper k alpha radiation
%q = q*1e-10; % change from m^-1 to A^-1 
    
%end



    