clear;
origHeight = 128;
origWidth = 128;
origFrames = 64;

height = 4;
width = 4;
frames = 4;

original = txt2rawBW('../foreman-128-128-64.txt', origHeight,origWidth,origFrames);

transform = zeros(size(original));
examineHaar2;
scale =2 ;
dSize = height*width*frames;


for fIdx = 1:origFrames/frames
    for wIdx = 1:origWidth/width
        for hIdx = 1:origHeight/height
            
            block = original((hIdx-1)*height+1 : hIdx*height,...
                             (wIdx-1)*width+1 : wIdx*width,...
                             (fIdx-1)*frames+1 : fIdx*frames);
            blockVec = block(:);
            transVec = BASIS2*blockVec;
                        
            
            % LLL
%             idcH = (hIdx-1)*height/2+1 : hIdx*height/2;
%             idcW = (wIdx-1)*width/2+1 : wIdx*width/2;
%             idcF = (fIdx-1)*frames/2+1 : fIdx*frames/2;
%             transform(idcH,idcW,idcF) = ...
%                 reshape(transVec(1:dSize/8),height/2,width/2,frames/2); % LLL

            if scale == 2 
                % LLL LLL
                idcH = (hIdx-1)*height/4+1 : hIdx*height/4;
                idcW = (wIdx-1)*width/4+1 : wIdx*width/4;
                idcF = (fIdx-1)*frames/4+1 : fIdx*frames/4;
                transform(idcH,idcW,idcF) = ...
                    reshape(transVec(1:dSize/8/8),height/4,width/4,frames/4); % LLL
                   % LLL LLH
                idcH = origHeight/4 + (hIdx-1)*height/4+1 : origHeight/4 + hIdx*height/4;
                idcW = (wIdx-1)*width/4+1 : wIdx*width/4;
                idcF = (fIdx-1)*frames/4+1 : fIdx*frames/4;
                transform(idcH,idcW,idcF) = ...
                    reshape(transVec(dSize/8/8+1:2*dSize/8/8),height/4,width/4,frames/4); % 
                % LLL LHL
                idcH = (hIdx-1)*height/4+1 : hIdx*height/4;
                idcW = origWidth/4 + (wIdx-1)*width/4+1 : origWidth/4 + wIdx*width/4;
                idcF = (fIdx-1)*frames/4+1 : fIdx*frames/4;           
                transform(idcH,idcW,idcF) = ...
                    reshape(transVec(2*dSize/8/8+1:3*dSize/8/8),height/4,width/4,frames/4); %               
                % LLL LHH
                idcH = origHeight/4 + (hIdx-1)*height/4+1 : origHeight/4 + hIdx*height/4;
                idcW = origWidth/4 + (wIdx-1)*width/4+1 : origWidth/4 + wIdx*width/4;
                idcF = (fIdx-1)*frames/4+1 : fIdx*frames/4;                        
                transform(idcH,idcW,idcF) = ... 
                    reshape(transVec(3*dSize/8/8+1:4*dSize/8/8),height/4,width/4,frames/4); %          
                % LLL HLL
                idcH = (hIdx-1)*height/2/2+1 : hIdx*height/2/2;
                idcW = (wIdx-1)*width/2/2+1 : wIdx*width/2/2;
                idcF = origFrames/2/2 + (fIdx-1)*frames/2/2+1 : origFrames/2/2 + fIdx*frames/2/2;
                transform(idcH,idcW,idcF) = ...
                    reshape(transVec(4*dSize/8/8+1:5*dSize/8/8),height/2/2,width/2/2,frames/2/2); %        
                % LLL HLH
                idcH = origHeight/2/2 + (hIdx-1)*height/2/2+1 : origHeight/2/2 + hIdx*height/2/2;
                idcW = (wIdx-1)*width/2/2+1 : wIdx*width/2/2;
                idcF = origFrames/2/2 + (fIdx-1)*frames/2/2+1 : origFrames/2/2 + fIdx*frames/2/2;
                transform(idcH,idcW,idcF) = ...
                    reshape(transVec(5*dSize/8/8+1:6*dSize/8/8),height/2/2,width/2/2,frames/2/2); %    
                % LLL HHL
                idcH = (hIdx-1)*height/2/2+1 : hIdx*height/2/2;
                idcW = origWidth/2/2 + (wIdx-1)*width/2/2+1 : origWidth/2/2 + wIdx*width/2/2;
                idcF = origFrames/2/2 + (fIdx-1)*frames/2/2+1 : origFrames/2/2 + fIdx*frames/2/2;
                transform(idcH,idcW,idcF) = ...
                    reshape(transVec(6*dSize/8/8+1:7*dSize/8/8),height/2/2,width/2/2,frames/2/2); %             
                % LLL HHH
                idcH = origHeight/2/2 + (hIdx-1)*height/2/2+1 : origHeight/2/2 + hIdx*height/2/2;
                idcW = origWidth/2/2 + (wIdx-1)*width/2/2+1 : origWidth/2/2 + wIdx*width/2/2;
                idcF = origFrames/2/2 + (fIdx-1)*frames/2/2+1 : origFrames/2/2 + fIdx*frames/2/2;
                transform(idcH,idcW,idcF) = ...
                    reshape(transVec(7*dSize/8/8+1:8*dSize/8/8),height/2/2,width/2/2,frames/2/2); %
            
            end            
            
            % LLH
            idcH = origHeight/2 + (hIdx-1)*height/2+1 : origHeight/2 + hIdx*height/2;
            idcW = (wIdx-1)*width/2+1 : wIdx*width/2;
            idcF = (fIdx-1)*frames/2+1 : fIdx*frames/2;
            transform(idcH,idcW,idcF) = ...
                reshape(transVec(dSize/8+1:2*dSize/8),height/2,width/2,frames/2); %
                       
            % LHL
            idcH = (hIdx-1)*height/2+1 : hIdx*height/2;
            idcW = origWidth/2 + (wIdx-1)*width/2+1 : origWidth/2 + wIdx*width/2;
            idcF = (fIdx-1)*frames/2+1 : fIdx*frames/2;           
            transform(idcH,idcW,idcF) = ...
                reshape(transVec(2*dSize/8+1:3*dSize/8),height/2,width/2,frames/2); %
                        
            % LHH
            idcH = origHeight/2 + (hIdx-1)*height/2+1 : origHeight/2 + hIdx*height/2;
            idcW = origWidth/2 + (wIdx-1)*width/2+1 : origWidth/2 + wIdx*width/2;
            idcF = (fIdx-1)*frames/2+1 : fIdx*frames/2;                        
            transform(idcH,idcW,idcF) = ... 
                reshape(transVec(3*dSize/8+1:4*dSize/8),height/2,width/2,frames/2); %
                       
            % HLL
            idcH = (hIdx-1)*height/2+1 : hIdx*height/2;
            idcW = (wIdx-1)*width/2+1 : wIdx*width/2;
            idcF = origFrames/2 + (fIdx-1)*frames/2+1 : origFrames/2 + fIdx*frames/2;
            transform(idcH,idcW,idcF) = ...
                reshape(transVec(4*dSize/8+1:5*dSize/8),height/2,width/2,frames/2); %
                        
            % HLH
            idcH = origHeight/2 + (hIdx-1)*height/2+1 : origHeight/2 + hIdx*height/2;
            idcW = (wIdx-1)*width/2+1 : wIdx*width/2;
            idcF = origFrames/2 + (fIdx-1)*frames/2+1 : origFrames/2 + fIdx*frames/2;
            transform(idcH,idcW,idcF) = ...
                reshape(transVec(5*dSize/8+1:6*dSize/8),height/2,width/2,frames/2); %
                        
            % HHL
            idcH = (hIdx-1)*height/2+1 : hIdx*height/2;
            idcW = origWidth/2 + (wIdx-1)*width/2+1 : origWidth/2 + wIdx*width/2;
            idcF = origFrames/2 + (fIdx-1)*frames/2+1 : origFrames/2 + fIdx*frames/2;
            transform(idcH,idcW,idcF) = ...
                reshape(transVec(6*dSize/8+1:7*dSize/8),height/2,width/2,frames/2); %
                        
            % HHH
            idcH = origHeight/2 + (hIdx-1)*height/2+1 : origHeight/2 + hIdx*height/2;
            idcW = origWidth/2 + (wIdx-1)*width/2+1 : origWidth/2 + wIdx*width/2;
            idcF = origFrames/2 + (fIdx-1)*frames/2+1 : origFrames/2 + fIdx*frames/2;
            transform(idcH,idcW,idcF) = ...
                reshape(transVec(7*dSize/8+1:8*dSize/8),height/2,width/2,frames/2); %
          
        
        end
    end
end
implay(uint8(scaleHaar(transform)),10);
h = haar3D(original,2);
implay(uint8(scaleHaar(h)),10);




