close all
A = load('noise1.txt');
x = log10(A(:,1));
% x = A(1:end,1);
y = A(1:end,2);

figure,
s = plot(x,y,'o-')%,...
        %'lineWidth',3,...
        %'MarkerSize',5);
%  xlabel('\sigma^2','FontSize',26,'FontWeight','bold');
xlabel('log \sigma^2')%,'FontSize',26,'FontWeight','bold');
ylabel('PSNR')%,'FontSize',26);%,'FontWeight','bold');
% title('lenna_{512,512}: 50% uniform mask');
hold on;
grid off;

%set(gca, 'FontSize', 24);

saveas(gcf, 'noise_lenna_wide.png');