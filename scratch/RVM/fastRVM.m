%% setup
clear,clc,clf

att_ = linspace(-1,1,500)'; % attributes
N = size(att_,1);

t = sin(2*pi*att_);           
sd = sqrt(0.2);
s = randn(size(t))*sd;
s = 0;
t = t + s;              % targets

% RBF kernel
M = N+1;
PHI = ones(N,M);      % Design matrix
r = 2;
for i = 1:N
    for j = 1:N
        PHI(i,j+1) = rbf(att_(i),att_(j),r);
    end
end
scatter(att_, t) % plot

%% Mem alloc
deltaML = zeros(M,1);
alphas = zeros(M,1);
alphas(:) = inf;
inModel = alphas == 1;
Sigma = zeros(M);
Mu = zeros(M,1);
numBasis = 0;
S = zeros(M,1);
Q = zeros(M,1);
s = zeros(M,1);
q = zeros(M,1);
tmp = zeros(N,1);
converged = false;
zeroFactor = 1e-12;
numIter = 0;
tmpTheta = zeros(M,1);
newModel = inModel;
MLThreshold = 0.001;
hist = zeros(1000,1);
%% RVM start. Init phase
% 1) Init sigma squared  - see below
% sig2 = 0.1;

% 2) Find initial basis
for m = 1:M
    tmp = PHI(:,m);
    deltaML(m) = ( (tmp' * t)/norm(tmp))^2;
end
[~, idx] = max(deltaML);
basisFn = PHI(:,idx);

% init sig2
sig2 = 0.1*(basisFn'*t)^2/norm(basisFn)^2;  % ensures that initial alpha > 0
sig2 = min(sig2, 0.1*var(t));
beta = 1/sig2;
%

inModel(idx) = true;
alphas(idx) = norm(basisFn)^2 / (((basisFn'*t)/norm(basisFn))^2 - sig2);
numBasis = numBasis + 1;

% 3) Compute initial stats - store in Sigma and Mu
Sigma(inModel,inModel) = 1/(alphas(idx)+norm(basisFn)^2/sig2);
Mu(inModel) = Sigma(inModel,inModel)*basisFn'*t / sig2; 
for m = 1:M
    tmp = PHI(:,m);
    S(m) = norm(tmp)^2/sig2 - ...
        tmp'*basisFn*Sigma(inModel,inModel)*basisFn'*tmp/sig2^2;
    Q(m) = tmp'*t/sig2 - ...
        tmp'*basisFn*Sigma(inModel,inModel)*basisFn'*t;
    if inModel(m)
        s(m) = alphas(m)*S(m)/(alphas(m)-S(m));
        q(m) = alphas(m)*Q(m)/(alphas(m)-S(m));
    else
        s(m) = S(m);
        q(m) = Q(m);
    end
end

%% Big loop
while ~converged
    numIter = numIter + 1;
    % 4) Select a candidate basis function
    % random for now - come back to this later
%     idx  = randi(M);
    % <later>pick basis function whos update gives largest change in ML
    deltaML(:) = 0;   % reset DeltaML
    for m = 1:M
        tmpTheta(m) = q(m)^2 - s(m);
        if (tmpTheta(m) > zeroFactor) && inModel(m)
            newAlpha = s(m)^2/(q(m)^2 - s(m));
            deltaML(m) = 0.5*(Q(m)^2/(S(m)+1/(1/newAlpha - 1/alphas(m))) ...
                - log(1 + S(m)*(1/newAlpha - 1/alphas(m))));
        elseif (tmpTheta(m) > zeroFactor) && ~inModel(m)
            deltaML(m) = 0.5*( (Q(m)^2-S(m))/S(m) + log(S(m)/Q(m)^2));
        elseif (tmpTheta(m) <= zeroFactor) && inModel(m)
            deltaML(m) = 0.5*(Q(m)^2/(S(m)-alphas(m)) ...
                - log(1 - S(m)/alphas(m)));
        else
            deltaML(m) = 0;
        end
    end
    [maxDML, idx] = max(deltaML);
    
    % 5) Compute theta
    theta = tmpTheta(idx);
%     theta = q(idx)^2 - s(idx);
    % 6,7,8) Adjust model and 10) Update statistics
    
    % 6) if in model and theta > 0 -> re-estimate
    if theta > zeroFactor && inModel(idx)
        % temporary vars
        newAlpha = s(idx)^2/(q(idx)^2 - s(idx));
        oldAlpha = alphas(idx);
        kj = 1/(Sigma(idx,idx) + 1/(newAlpha-oldAlpha));
        Sj = Sigma(inModel, idx);
        oldMu = Mu(idx);
        % updates
        alphas(idx) = newAlpha;
        Sigma(inModel,inModel) = Sigma(inModel,inModel) - kj*(Sj*Sj');
        Mu(inModel) = Mu(inModel) - kj*Mu(idx)*Sj;
        for m = 1:M
            tmp = PHI(:,m);
            S(m) = S(m) + kj*(Sj'*PHI(:,inModel)'*tmp/sig2)^2;
            Q(m) = Q(m) + kj*oldMu*(Sj'*PHI(:,inModel)'*tmp/sig2); %**
        end
        s(~inModel) = S(~inModel);
        q(~inModel) = Q(~inModel);
        s(inModel) = alphas(inModel).*S(inModel)./(alphas(inModel)-S(inModel));
        q(inModel) = alphas(inModel).*Q(inModel)./(alphas(inModel)-S(inModel));
        hist(numIter) = 0;
        
    % 7) if not in model and theta > 0 -> add to model 
    elseif theta > zeroFactor && ~inModel(idx)
        % temporary vars
        basisFn = PHI(:,idx);       % new basis function
        newAlpha = s(idx)^2/(q(idx)^2 - s(idx));
        tmpSig = Sigma(inModel,inModel);
        Sii = 1/(newAlpha + S(idx));
        Mi = Sii*Q(idx);
        ei = basisFn - PHI(:,inModel)*Sigma(inModel,inModel)*...
            PHI(:,inModel)'*basisFn/sig2;
        % updates
        Sigma(idx,idx) = Sii;
        Sigma(inModel,inModel) = Sigma(inModel,inModel)+...
            Sii*tmpSig*PHI(:,inModel)'*...
            (basisFn*basisFn')*PHI(:,inModel)*tmpSig/sig2^2;
        Sigma(inModel,idx) = -Sii*tmpSig*PHI(:,inModel)'*basisFn/sig2^2;
        Sigma(idx,inModel) = -Sii*(tmpSig*PHI(:,inModel)'*basisFn)'/sig2^2;
        Mu(inModel) = Mu(inModel) - Mi*tmpSig*PHI(:,inModel)'*basisFn/sig2;
        Mu(idx) = Mi;
        inModel(idx) = true;
        alphas(idx) = newAlpha;
        numBasis = numBasis + 1;
        for m = 1:M
            tmp = PHI(:,m);
            S(m) = S(m) - Sii*(tmp'*ei/sig2)^2;
            Q(m) = Q(m) - Mi*(tmp'*ei/sig2);
        end
        s(~inModel) = S(~inModel);
        q(~inModel) = Q(~inModel);
        s(inModel) = alphas(inModel).*S(inModel)./(alphas(inModel)-S(inModel));
        q(inModel) = alphas(inModel).*Q(inModel)./(alphas(inModel)-S(inModel));
        hist(numIter) = 1;
        % clean up
        clear tmpSig;
        
    % 8) if in model and theta <= 0 -> delete from model
    elseif theta <= zeroFactor && inModel(idx)
        % temporary variables
        Sii = Sigma(idx,idx);
        Mi = Mu(idx);
        Sj = Sigma(inModel,idx);
        % updates
        Sigma(inModel,inModel) = Sigma(inModel,inModel) + (Sj*Sj')/Sii;
        Mu(inModel) = Mu(inModel) - Mi*Sj/Sii;        
        Mu(idx) = 0;
        Sigma(idx,inModel) = 0;
        Sigma(inModel,idx) = 0;
        
        for m = 1:M
            tmp = PHI(:,m);
            S(m) = S(m) + (Sj'*PHI(:,inModel)'*tmp/sig2)^2/Sii;
            Q(m) = Q(m) + (Sj'*PHI(:,inModel)'*tmp/sig2)*Mi/Sii;
        end
        inModel(idx) = false;
        alphas(idx) = inf;
        numBasis = numBasis - 1;
        s(~inModel) = S(~inModel);
        q(~inModel) = Q(~inModel);
        s(inModel) = alphas(inModel).*S(inModel)./(alphas(inModel)-S(inModel));
        q(inModel) = alphas(inModel).*Q(inModel)./(alphas(inModel)-S(inModel));        
        hist(numIter) = -1;
    end
    
    % 9) NA since fixed sig2
    % 10) Update Statistics - see above
    
    % 11) check for convergence
    if maxDML < MLThreshold
%     if numIter > 50
        converged = true;
    end
%     fprintf('Paused. Iteration %d. Hist = %d. Press enter to continue\n',...
%        numIter,hist(numIter));
   % pause;
end

MuOut = Mu(inModel);
SigmaOut = Sigma(inModel,inModel);
BasisOut = PHI(:,inModel);

% Results
Pred = BasisOut*MuOut;
hold on;
plot(att_,Pred);
scatter(att_(inModel(2:end)),t(inModel(2:end)),'rd')




