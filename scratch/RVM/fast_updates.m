%% setup
clear,clc,clf

att_ = linspace(-1,1,50)'; % attributes
N = size(att_,1);

targets = sin(2*pi*att_);           
sd = sqrt(0);
noise = randn(size(targets))*sd;
%noise = 0;
targets = targets + noise;              % targets

% RBF kernel
% M = N+1;
M = 4;
BASIS = ones(N,M);      % Design matrix
BASIS = [sin(att_),att_.^3,att_.^5,cos(att_)];
% r = 2;
% for i = 1:N
%     for j = 1:N
%         BASIS(i,j+1) = rbf(att_(i),att_(j),r);
%     end
% end
scatter(att_, targets) % plot
hold on;

%% Start
fprintf('Relevance Vector Machine:\n');

change_in_ML_threshold = 0.01;
noiseStd = 0.1;
zeroFactor = 1.0e-12;
alpha = 0;
beta = 0;
gamma = zeros(M,1);
in_model = 0;
phi_column = zeros(N);
scaling = zeros(M);
%% Call to initialize
alpha_min = 1.0e-3;
alpha_max = 1.0e3;
for basis = 1:M
    column = BASIS(:,basis);
    scaling(basis) = norm(column);
    if scaling(basis) == 0
        scaling(basis) = 1;
    end
    BASIS(:,basis) = BASIS(:,basis)/scaling(basis);
end

beta = 1/(noiseStd*noiseStd);
max_projection = 0;

for b = 1:M
    column = BASIS(:,b);
    projection = column'*targets;
    if abs(projection) > abs(max_projection)
        max_projection = projection;
        in_model = b;
    end
end
phi_column = BASIS(:,in_model);
% calculate alpha
p = beta*(phi_column'*phi_column);
q = beta*(phi_column'*targets);
alpha = p*p/(q*q-p);
if alpha < 0
    alpha = alpha_max;
end
clear column 
fprintf('Chosen init basis: %d\n',in_model);
fprintf('Initial alpha: %f\n', alpha);
% End of init.
%% Declarations
fprintf('End of initialization\n');
PHI = zeros(N,M);
alphas = zeros(M,1);
alphas(in_model) = alpha;
indeces_used = zeros(M,1);
ins_model = false(M,1);
where_is = zeros(M,1);
indeces_used(1) = in_model;
where_is(in_model) = 1;
ins_model(in_model) = true;
included_basis = 1;
PHI(:,included_basis) = phi_column;
mean_ = zeros(M,1);
covariance = zeros(M);
S_in = zeros(M,1);
Q_in = zeros(M,1);
S_out = zeros(M,1);
Q_out = zeros(M,1);
relevance_factors = zeros(M,1);
BASIS_transpose = BASIS';
BASIS_PHI = BASIS_transpose*PHI;
BETA_BASIS_PHI = beta*BASIS_PHI;
BASIS_Targets = BASIS_transpose*targets;
%% Call to full_statistics
temp_cova = zeros(included_basis);
temp_temp_cova = zeros(included_basis);
temp_PHI = zeros(N,included_basis);
temp_PHI_transpose = zeros(included_basis,N);
temp_PHI = PHI(1:N,1:included_basis);
temp_PHI_transpose = temp_PHI';
temp_temp_cova = temp_PHI_transpose*temp_PHI;
for r=1:included_basis
    for c = 1:included_basis
        if r==c
            temp_temp_cova(r,c) = beta*temp_temp_cova(r,c)...
                +alphas(indeces_used(r));
        else
            temp_temp_cova(r,c) = beta*temp_temp_cova(r,c);
        end
    end
end
choles = zeros(included_basis);
inv_chol = zeros(included_basis);
choles = chol(temp_temp_cova);
inv_chol = inv(choles);
tran_inv_chol = inv_chol';
temp_cova = inv_chol*tran_inv_chol;
temp_mean = zeros(included_basis,1);
temp_temp_mean = zeros(included_basis,1);
temp_temp_mean = temp_PHI_transpose*targets;
temp_mean = temp_cova*temp_temp_mean;
temp_mean = beta*temp_mean;
y = zeros(N,1);
y = temp_PHI*temp_mean;
residual = zeros(N,1);
residual = targets - y;
% calculating sparsity and quality factors
sum_inv_chol = zeros(included_basis,1);
for r=1:included_basis
    for c=1:included_basis
        sum_inv_chol(r) = sum_inv_chol(r) + inv_chol(r,c)*inv_chol(r,c);
    end
end
for r=1:included_basis
    gamma(r) = 1 - alphas(indeces_used(r))*sum_inv_chol(r);
end
BETA_chol = zeros(M,included_basis);
BETA_BASIS_PHI = beta*BASIS_PHI;
BETA_chol = BETA_BASIS_PHI*inv_chol;
BETA_chol = BETA_chol.*BETA_chol;
sums = zeros(M,1);
for r = 1:M
    for c = 1:included_basis
        sums(r) = sums(r) + BETA_chol(r,c);
    end
end
for b = 1:M
    column_basis_phi = zeros(included_basis,1);
    for r = 1:included_basis
        column_basis_phi(r) = BASIS_PHI(b,r);
    end
    S_in(b) = beta - sums(b);
    Q_in(b) = beta*BASIS_Targets(b) - beta*column_basis_phi'*temp_mean;
    S_out(b) = S_in(b);
    Q_out(b) = Q_in(b);
    if ins_model(b)
        S_out(b) = alphas(b)*S_in(b)/(alphas(b) - S_in(b));
        Q_out(b) = alphas(b)*Q_out(b)/(alphas(b) - S_in(b));
    end
    relevance_factors(b) = Q_out(b)*Q_out(b) - S_out(b);
end
for r = 1:included_basis
    mean_(r) = temp_mean(r);
end
for r = 1:included_basis
    for c = 1:included_basis
        covariance(r,c) = temp_cova(r,c);
    end
end
for r = 1:N
    for c = 1:included_basis
        PHI(r,c) = temp_PHI(r,c);
    end
end
clear residual y temp_mean temp_temp_mean sums
clear sum_inv_chol choles BETA_chol inv_chol tran_inv_chol
clear temp_temp_cova temp_cova temp_PHI temp_PHI_transpose
% end of full_statistics
added_basis = 0;
count = 0;
breaker = false;
%% Start the Big Loop
iterations = 0;
last_iteration = false;
while ~last_iteration
   iterations = iterations + 1;
   UpdateIteration = true;
   anyToAdd = false;
   add_basis = 1;
   % Compute the change in lklihood for all possible updates and basis
   % functions
   DeltaML = zeros(M,1);
   Action = zeros(M,1);
   for b = 1:M
       if relevance_factors(b) > 1e-12 && ~(ins_model(b))
           quot = Q_in(b)*Q_in(b) / S_in(b);
           if quot < 0
               breaker = true;
           end
           DeltaML(b) = (quot - 1 -log(quot))/2;
           Action(b) = add_basis;
           anyToAdd = true;
       end 
   end
   DeltaML = real(DeltaML);
   if breaker
   %    break
   end
   max_DeltaML = 0;
   chosen_basis = 0;
   selected_action = 0;
   for b = 1:M
       if DeltaML(b) > max_DeltaML
           max_DeltaML = DeltaML(b);
           chosen_basis = b;
           selected_action = Action(b);
       end
   end
   if chosen_basis == 0
       chosen_basis = randi(M);
   end
   % get the new basis function and calculate the new alpha for that
   % particular basis function
   current_basis_function = BASIS(:,chosen_basis);
   new_alpha = S_out(chosen_basis)*S_out(chosen_basis)/relevance_factors(chosen_basis);
   if selected_action == add_basis
        fprintf('Iteration %d Adding %d\n',iterations,chosen_basis);
        indeces_used(included_basis+1) = chosen_basis;
        where_is(chosen_basis) = included_basis+1;
        ins_model(chosen_basis) = true;
        BASIS_phi = BASIS_transpose*current_basis_function;
        BASIS_PHI(:,included_basis+1) = BASIS_phi;
        B_phi = beta*current_basis_function;
        BETA_BASIS_phi = beta*BASIS_phi;
        temp1 = B_phi'*PHI(:,1:included_basis);
        temp2 = temp1*covariance(1:included_basis,1:included_basis);
        alphas(chosen_basis) = new_alpha;
        PHI(:,included_basis+1) = current_basis_function;
        s_ii = 1/(new_alpha + S_in(chosen_basis));
        s_i = -s_ii*temp2;
        TAU = zeros(included_basis);
        TAU = s_i'*temp2;
        for r=1:included_basis
            for c = 1:included_basis
                covariance(r,c) = covariance(r,c) - TAU(r,c);
            end
        end
        covariance(included_basis+1,1:included_basis) = s_i; %% BUG, *beta is missing
        covariance(1:included_basis,included_basis+1) = s_i; %% BUG, see above
        covariance(included_basis+1,included_basis+1) = s_ii;
        mu_i =s_ii*Q_in(chosen_basis);
        delta_Mu = zeros(included_basis+1,1);
        for r=1:included_basis
            delta_Mu(r) = -mu_i*temp2(r);
        end
        delta_Mu(included_basis+1) = mu_i;
        for r = 1:included_basis
            mean_(r) = mean_(r) + delta_Mu(r);
        end
        mean_(included_basis+1) = delta_Mu(included_basis+1);
        temp3 = zeros(M,1);
        temp3 = BETA_BASIS_PHI(:,1:included_basis)*temp2';
        mCI = zeros(M,1);
        mCI = BETA_BASIS_phi - temp3;
        S_in = S_in - s_ii*mCI.*mCI;
        Q_in = Q_in - mu_i*mCI;
        clear BETA_BASIS_phi mCI temp3 delta_Mu TAU s_i temp1 temp2 B_phi;
        clear BASIS_phi;
        included_basis = included_basis + 1;
        added_basis = added_basis + 1;
   end
   for b=1:M
       S_out(b) = S_in(b);
       Q_out(b) = Q_in(b);
       if ins_model(b)
           S_out(b) = alphas(b)*S_in(b)/(alphas(b) - S_in(b));
           Q_out(b) = alphas(b)*Q_in(b)/(alphas(b) - S_in(b));
       end
       relevance_factors(b) = Q_out(b)*Q_out(b) - S_out(b);
   end
   BETA_BASIS_PHI(:,1:included_basis) = beta*BASIS_PHI(:,1:included_basis);
   for r = 1:included_basis
       gamma(r) = 1 - alphas(indeces_used(r))*covariance(r,r);
   end
   fprintf('Change in Likelihood: %f\n',max_DeltaML);
   if max_DeltaML < change_in_ML_threshold
       last_iteration = true;
   end
   clear current_basis_function;
end
fprintf('Added %d\n',added_basis);
coefficients = zeros(M,1);
for r = 1:M
    if ins_model(r)
        coefficients(r) = mean_(where_is(r));
    end
end

pred = BASIS*coefficients;
plot(att_,pred);
