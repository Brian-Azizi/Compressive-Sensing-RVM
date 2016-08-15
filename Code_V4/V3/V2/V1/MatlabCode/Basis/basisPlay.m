signalTxt = load('../test-4-8-2.txt');
%signalTxt = load('../test-4-4-4.txt');
%signalTxt = load('akiyo-16-8-64.txt');
%signalTxt = load('akiyo-32-16-64.txt');

height = 4;
width = 8;
frames = 2;
signal = zeros(height,width, frames);
for k = 1:frames
    signal(:,:,k) = signalTxt(height*(k-1)+1:height*k,:);
end

signalVector = signal(:);

haar = haar3D(signal);
%haarVector = haar(:);   %% This is not how its done!!
haarVector = zeros(height*width*frames,1);

cubeSize = height*width*frames / 8;
tmpCube = zeros(height/2,width/2,frames/2);

tmpCube = haar(1:height/2,1:width/2,1:frames/2); % LLL
haarVector(1:cubeSize) = tmpCube;
tmpCube = haar(height/2+1:height,1:width/2,1:frames/2); % LLH
haarVector(cubeSize+1:2*cubeSize) = tmpCube;
tmpCube = haar(1:height/2,width/2+1:width,1:frames/2); % LHL
haarVector(2*cubeSize+1:3*cubeSize) = tmpCube;
tmpCube = haar(height/2+1:height,width/2+1:width,1:frames/2); % LHH
haarVector(3*cubeSize+1:4*cubeSize) = tmpCube;
tmpCube = haar(1:height/2,1:width/2,frames/2+1:frames); % HLL
haarVector(4*cubeSize+1:5*cubeSize) = tmpCube;
tmpCube = haar(height/2+1:height,1:width/2,frames/2+1:frames); % HLH
haarVector(5*cubeSize+1:6*cubeSize) = tmpCube;
tmpCube = haar(1:height/2,width/2+1:width,frames/2+1:frames); % HHL
haarVector(6*cubeSize+1:7*cubeSize) = tmpCube;
tmpCube = haar(height/2+1:height,width/2+1:width,frames/2+1:frames); % HHH
haarVector(7*cubeSize+1:8*cubeSize) = tmpCube;
        

%haarBasis;
BASIS = haarBasis(height,width,frames);

altHaar = BASIS * signalVector;

error = sum(abs(haarVector - altHaar));
fprintf('Total SAR = %f\n',error);
