

att = linspace(-2*pi,2*pi,200)';
N = size(att,1);

t = sin(att);
sd = sqrt(0.2);
s = randn(size(t))*sd;
t = t + s;

% % OLS
% X = [ones(size(att,1),1),att];
% w = pinv(X'*X)*X'*t;
% t_hat = X*w;
% figure;
% scatter(X(:,2:end),t);
% hold on
% plot(X(:,2:end),t_hat);
% 
% % Basis functions
% PHI = [ones(size(att,1),1), att, att.^2, att.^3, sin(att)];
% wP = pinv(PHI'*PHI)*PHI'*t;
% tP = PHI*wP;
% figure;
% scatter(att,t);
% hold on;
% plot(att,tP);
% 
% RBF kernel
M = N+1;
K = ones(N,M);
r = 1;
for i = 1:N
    for j = 1:N
        K(i,j+1) = rbf(att(i),att(j),r);
    end
end

% RVM startfor qq = 1:20
% 1) initializa alpha, beta, Threshold;
TH = 1e8;
support = ones(1,M);
alpha = ones(M,1);
beta = 1;
% 2) iterate
converged = false;
counter = 0;
while(~converged)
    counter = counter + 1;
    PHI = K(:,support == 1);
    A = diag(alpha(support == 1));
    SIG = pinv(A + beta*PHI'*PHI);
    m = beta*SIG*PHI'*t;
    gamma = 1 - alpha(support ==1).*diag(SIG);
    alpha(support == 1) = gamma./(m.*m);
    beta = (N-sum(gamma))/(norm(t-PHI*m)^2);
    for j = 1:M
        if support(j) == 1
            if alpha(j) > TH
                support(j) = 0;
                alpha(j) = inf;
            end
        end
    end
    if counter == 20*N
        converged = true;
    end
end

fprintf('Sparsity: %d\n',sum(support));
t_pred = K(:,support ==1 )*m;
sig2 = 1/beta + diag(K(:,support==1)*SIG*K(:,support==1)');
t_plus = t_pred + sqrt(sig2);
t_minus = t_pred - sqrt(sig2);

% wOLS = pinv(K'*K)*K'*t;
% tK = K*wOLS;
figure
scatter(att,t);
hold on;
plot(att,t_pred,'k');
scatter(att(support(2:end)==1),t(support(2:end)==1),'rd');
plot(att,t_plus,'g-');
plot(att,t_minus,'g-');