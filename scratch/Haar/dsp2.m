fSampling = 2000;
tSampling = 1/fSampling;
t = -0.005:tSampling:0.005;
xt = rectpuls(t,0.004);
stem(t,xt);
xlabel('time (sec)');
ylabel('x(t)')
title('Signal in time domain');

nFFT = 64;
xF = fft(xt,nFFT);

fSpacing = fSampling/nFFT;
fNyquist = fSampling/2;

fStart = -fNyquist;
fEnd = fNyquist - fSpacing;
fAxis = fStart:fSpacing:fEnd;

magXF = abs(xF);
phaseXF = angle(xF);
phaseXF = unwrap(phaseXF);

magXF = fftshift(magXF);
phaseXF = fftshift(phaseXF);

figure, stem(fAxis,magXF);
xlabel('Frequency (Hz)');
ylabel('Magnitude X(F)');

figure, stem(fAxis,phaseXF);
xlabel('Frequency (Hz)');
ylabel('Phase X(F) (rad)');