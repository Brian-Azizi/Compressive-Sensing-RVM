clear, clc, clf;
orig = load('targets.txt');
pred = load('pred.txt');
plot(orig);
hold on;
plot(pred);
