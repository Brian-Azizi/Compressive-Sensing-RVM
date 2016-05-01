%% setup
clear,clc,clf

att_ = linspace(-1,1,200)'; % attributes
N = size(att_,1);

t = sin(2*pi*att_);           
sd = sqrt(0.2);
noise = randn(size(t))*sd;
noise = 0;
t = t + noise;              % targets

% RBF kernel
M = 4;
PHI = ones(N,M);      % Design matrix
% r = 2;
% for i = 1:N
%     for j = 1:N
%         PHI(i,j+1) = rbf(att_(i),att_(j),r);
%     end
% end

scatter(att_, t) % plot



%% Memory Allocation
tmp = zeros(N,1);
deltaML = zeros(M,1);
alphas = zeros(M,1);
alphas(:) = inf;
inModel = false(M,1); % initialize to false
numBasis = 0;

basisIDX = zeros(M,1);
where_is = zeros(M,1);
Mu = zeros(M,1);
Sigma = zeros(M);
S = zeros(M,1);
Q = zeros(M,1);
s = zeros(M,1);
q = zeros(M,1);
curBasis = zeros(N,M);

converged = false;
zeroFactor = 1e-12;
numIter = 0;
MLThreshold = 0.1;

tmpTheta = zeros(M,1);
%% Initiation Phase
%1) initiate sigma^2
% sig2 = 0.1*var(t) ;
sig2 = 1;
beta = 1/sig2;

%1.2) Normalize basis functions
scaling = zeros(1,M);
for m = 1:M
    tmp = PHI(:,m);
    scaling(m) = norm(tmp);
    PHI(:,m) = tmp/scaling(m);
end

%2) Find initial basis (whose update gives maximum change in ML)
for m = 1:M;
    tmp = PHI(:,m);
    deltaML(m) = (tmp'*t)^2/norm(tmp)^2;
end
[maxDML, init_idx] = max(deltaML);
basisFn = PHI(:,init_idx);
fprintf('MaxDML = %f\n',maxDML);
% init sig2
% sig2 = 0.1*(basisFn'*t)^2/norm(basisFn)^2;  % ensures that initial alpha > 0
% sig2 = min(sig2, 0.1*var(t));
% beta = 1/sig2;
%
alpha_max = 1e3;
alpha = norm(basisFn)^2/((basisFn'*t)^2/norm(basisFn)^2 - sig2);
if alpha < 0
    alpha = 1;%alpha_max;
end
alphas(init_idx) = alpha;
inModel(init_idx) = true;
numBasis = numBasis + 1; 

%3) Compute initial stats
basisIDX(numBasis) = init_idx;  % assert: numBasis == 1
where_is(init_idx) = numBasis;
Sigma(numBasis,numBasis) = 1/(alpha + beta*(basisFn'*basisFn));
Mu(1) = beta*Sigma(1)*basisFn'*t;
curBasis(:,numBasis) = basisFn;

for m = 1:M
    tmp = PHI(:,m);
    S(m) = beta*(tmp'*tmp) - beta^2*(tmp'*basisFn)*Sigma(1,1)*(basisFn'*tmp);
    Q(m) = beta*(tmp'*t) - beta^2*(tmp'*basisFn)*Sigma(1,1)*(basisFn'*t);
end
s(~inModel) = S(~inModel);
q(~inModel) = Q(~inModel);
s(init_idx) = alpha*S(init_idx)/(alpha - S(init_idx));
q(init_idx) = alpha*Q(init_idx)/(alpha - Q(init_idx));


%% Big Loop
while ~converged
    numIter = numIter + 1;
    
    %4) Select a basis function
    deltaML(:) = 0;
    for m = 1:M
        tmpTheta(m) = q(m)^2 - s(m);
%         if inModel(m) && (tmpTheta(m) > zeroFactor) % update
%             alpha = s(m)^2/(q(m)^2 - s(m));
%             deltaML(m) = 0.5* (Q(m)^2/(S(m)+(1/alpha - 1/alphas(m))^(-1))...
%                 -log(1+S(m)*(1/alpha-1/alphas(m))));
%             
        if ~inModel(m) && (tmpTheta(m) > zeroFactor) % add
            alpha = s(m)^2/(q(m)^2 - s(m));
            deltaML(m) = 0.5* ((Q(m)^2-S(m))/S(m) + log(S(m)/Q(m)^2));
        
%         elseif inModel(m) && (tmpTheta(m) <= zeroFactor) % delete
%             deltaML(m) = 0.5* (Q(m)^2/(S(m)-alphas(m)) - ...
%                 log(1 - S(m)/alphas(m)));
        else
            deltaML(m) = 0;
        end
    end
    [maxDML, idx] = max(deltaML);
    if maxDML < 0
        error('maxDML < 0')
    elseif maxDML == 0
        idx = randi(M);
        while deltaML(idx) < 0
            idx = randi(M);
        end
    end
    
    %5) Compute theta
    theta = tmpTheta(idx);
%      assert(false)
    %6-8) Adjust model
%     if inModel(idx) && (theta > zeroFactor) % re-estimate
%         fprintf('update\n');
%         % temps
%         jj = invBasisIDX(idx);
%         Sjj = Sigma(jj,jj);
%         Sj = Sigma(1:numBasis,jj);
%         mj = Mu(jj);
%         alpha = s(idx)^2/(q(idx)^2 - s(idx));
%         oldAlpha = alphas(idx);
%         kj = (Sjj + 1/(alpha-oldAlpha))^(-1);
%         tmpSig = Sigma(1:numBasis,1:numBasis);
%         tmpMu = Mu(1:numBasis);
%         tmpBasis = curBasis(:,1:numBasis);
%         
%         % Sigma
%         Sigma(1:numBasis,1:numBasis) = tmpSig - kj*(Sj*Sj');
%         % Mu
%         Mu(1:numBasis) = tmpMu - kj*mj*Sj;
%         % Alpha
%         alphas(idx) = alpha;
%         
%         % parameters 
%         % ~
%         % S,Q,s and q
%         for m = 1:M
%             tmp = PHI(:,idx);
%             S(m) = S(m) + kj*(beta*Sj'*tmpBasis'*tmp)^2;
%             Q(m) = Q(m) + kj*mj*(beta*Sj'*tmpBasis'*tmp);
%         end
%         s(~inModel) = S(~inModel);
%         q(~inModel) = Q(~inModel);
%         s(inModel) = alphas(inModel).*S(inModel)./(alphas(inModel)-S(inModel));
%         q(inModel) = alphas(inModel).*Q(inModel)./(alphas(inModel)-S(inModel));
%         assert(false)
    if ~inModel(idx) && (theta > zeroFactor) % add
        fprintf('add\n');
        % temps
        basisFn = PHI(:,idx);
        alpha = s(idx)^2/(q(idx)^2 - s(idx));    
        Sii = (alpha+S(idx))^(-1);
        mi = Sii*Q(idx);
        tmpBasis = curBasis(:,1:numBasis);
        tmpSig = Sigma(1:numBasis, 1:numBasis);
        tmpMu = Mu(1:numBasis);
        ei = basisFn - beta*tmpBasis*tmpSig*tmpBasis'*basisFn;
        
        % Sigma
        Sigma(1:numBasis,1:numBasis) = tmpSig+...
            beta^2*Sii*tmpSig*tmpBasis'*(basisFn*basisFn')*tmpBasis*tmpSig;
        Sigma(1:numBasis,numBasis+1) = -beta^2*Sii*tmpSig*tmpBasis'*basisFn;
        Sigma(numBasis+1,1:numBasis) = -beta^2*Sii*(tmpSig*tmpBasis'*basisFn)';
        Sigma(numBasis+1,numBasis+1) = Sii;
        
        % Mu
        Mu(1:numBasis) = tmpMu - mi*beta*tmpSig*tmpBasis'*basisFn;
        Mu(numBasis+1) = mi;
        
        % Alpha
        alphas(idx) = alpha;            
        
        % parameters
        numBasis = numBasis + 1;
        curBasis(:,numBasis) = basisFn;
        basisIDX(numBasis) = idx;
        where_is(idx) = numBasis;
        inModel(idx) = true;
% assert(false);
        % S,Q,s and q
        for m = 1:M
           tmp = PHI(:,m);
           S(m) = S(m) - Sii*(beta*tmp'*ei)^2;
           Q(m) = Q(m) - mi*(beta*tmp'*ei);
        end
        
    end
        s(~inModel) = S(~inModel);
        q(~inModel) = Q(~inModel);
        s(inModel) = alphas(inModel).*S(inModel)./(alphas(inModel)-S(inModel));
        q(inModel) = alphas(inModel).*Q(inModel)./(alphas(inModel)-S(inModel));
             
%     elseif inModel(idx) && (theta <= zeroFactor) % delete
%         fprintf('delete\n');
%         alpha = inf;
%         alphas(idx) = alpha;
%         inModel(idx) = false;
%         numBasis = numBasis - 1;
%     else
%         fprintf('Nothing\n');
%     end
    
    if numBasis == 3
        Sigma(1:numBasis,1:numBasis)
        chol(Sigma(1:numBasis-1,1:numBasis-1))
    end
   
    
    if maxDML < MLThreshold
        converged = true;
    end
end

mean = zeros(M,1);
mean(where_is(inModel)) = Mu(1:numBasis);
pred = PHI*(mean.*scaling');
hold on;
plot(att_,pred);
