close all;
% figure;
n = 501;
b=ceil(0.3*n);
N = 4;

P = ones((n+b)*N+b)*0.7;
for i=0:N-1
    for j=0:N-1
        [u,v] = meshgrid(linspace(0,N,n));
        if i < N/2 && j < N/2
            F = haarFather(u/2-j).*haarFather(v/2-i);
        elseif i < N/2 && ~(j < N/2)
            F = haarFather(u/2-j+N/2).*haarMother(v/2-i);
        elseif ~(i < N/2) && j < N/2
            F = haarMother(u/2-j).*haarFather(v/2-i+N/2);
        else
            F = haarMother(u/2-j+N/2).*haarMother(v/2-i+N/2);
        end
        %F = cos(pi*(2*u+1)*i*0.5/N).*cos(pi*(2*v+1)*j*0.5/N);
        P((b+(n+b)*i+1):(b+(n+b)*i+n),(b+(n+b)*j+1):(b+(n+b)*j+n)) = F;
        
%         subplot(8,8,8*i+j+1);
%         imagesc(F); colormap(gray);
%         axis off;
%         axis square;
    end
end

% figure, imagesc(P), colormap(gray), axis off, axis square;

imwrite(unify(P),'haar2_scale1.png');

P = ones((n+b)*N+b)*0.7;
for i=0:N-1
    for j=0:N-1
        [u,v] = meshgrid(linspace(0,N,n));
        if i < N/2 && j < N/2
            if i < N/4 && j < N/4
                F = haarFather(u/4-j).*haarFather(v/4-i);
            elseif i < N/4 && ~(j < N/4)
                F = haarFather(u/4-j+N/4).*haarMother(v/4-i);
            elseif ~(i < N/4) && j < N/4
                F = haarMother(u/4-j).*haarFather(v/4-i+N/4);
            else
                F = haarMother(u/4-j+N/4).*haarMother(v/4-i+N/4);
            end
        
        elseif i < N/2 && ~(j < N/2)
            F = haarFather(u/2-j+N/2).*haarMother(v/2-i);
        elseif ~(i < N/2) && j < N/2
            F = haarMother(u/2-j).*haarFather(v/2-i+N/2);
        else
            F = haarMother(u/2-j+N/2).*haarMother(v/2-i+N/2);
        end
        %F = cos(pi*(2*u+1)*i*0.5/N).*cos(pi*(2*v+1)*j*0.5/N);
        P((b+(n+b)*i+1):(b+(n+b)*i+n),(b+(n+b)*j+1):(b+(n+b)*j+n)) = F;
        
%         subplot(8,8,8*i+j+1);
%         imagesc(F); colormap(gray);
%         axis off;
%         axis square;
    end
end

% figure, imagesc(P), colormap(gray), axis off, axis square;
imwrite(unify(P),'haar2_scale2.png');

% 
% P = ones((n+b)*N+b)*0.7;
% for i=0:N-1
%     for j=0:N-1
%         [u,v] = meshgrid(linspace(0,N,n));
%         if i < N/2 && j < N/2
%             if i < N/4 && j < N/4
%                 if i < N/8 && j < N/8
%                     F = haarFather(u/8-j).*haarFather(v/8-i);
%                 elseif i < N/8 && ~(j < N/8)
%                     F = haarFather(u/8-j+N/8).*haarMother(v/8-i);
%                 elseif ~(i < N/8) && j < N/8
%                     F = haarMother(u/8-j).*haarFather(v/8-i+N/8);
%                 else
%                     F = haarMother(u/8-j+N/8).*haarMother(v/8-i+N/8);
%                 end
%                 
%             elseif i < N/4 && ~(j < N/4)
%                 F = haarFather(u/4-j+N/4).*haarMother(v/4-i);
%             elseif ~(i < N/4) && j < N/4
%                 F = haarMother(u/4-j).*haarFather(v/4-i+N/4);
%             else
%                 F = haarMother(u/4-j+N/4).*haarMother(v/4-i+N/4);
%             end
%         
%         elseif i < N/2 && ~(j < N/2)
%             F = haarFather(u/2-j+N/2).*haarMother(v/2-i);
%         elseif ~(i < N/2) && j < N/2
%             F = haarMother(u/2-j).*haarFather(v/2-i+N/2);
%         else
%             F = haarMother(u/2-j+N/2).*haarMother(v/2-i+N/2);
%         end
%         %F = cos(pi*(2*u+1)*i*0.5/N).*cos(pi*(2*v+1)*j*0.5/N);
%         P((b+(n+b)*i+1):(b+(n+b)*i+n),(b+(n+b)*j+1):(b+(n+b)*j+n)) = F;
%         
% %         subplot(8,8,8*i+j+1);
% %         imagesc(F); colormap(gray);
% %         axis off;
% %         axis square;
%     end
% end
% 
% % figure, imagesc(P), colormap(gray), axis off, axis square;
% imwrite(unify(P),'haar2_scale3.png');

