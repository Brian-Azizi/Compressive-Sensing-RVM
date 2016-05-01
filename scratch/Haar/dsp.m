clear all;
close all;

% Sampling frequency
fSampling = 4000;

% Amplitude
A = 0.01;

% Create time vector
tSampling = 1/fSampling;
t = 0:tSampling:0.5;

% set of note frequencies, 2nd octave, Do Re Mi Fa So La Ti
fNote = [524 588 660 698 784 880 988];

% Generate notes
Do = A*sin(2*pi*fNote(1)*t + 2*pi*rand);%C
Re = A*sin(2*pi*fNote(2)*t + 2*pi*rand);%D
Mi = A*sin(2*pi*fNote(3)*t + 2*pi*rand);%E
Fa = A*sin(2*pi*fNote(4)*t + 2*pi*rand);%F
So = A*sin(2*pi*fNote(5)*t + 2*pi*rand);%G
La = A*sin(2*pi*fNote(6)*t + 2*pi*rand);%A
Ti = A*sin(2*pi*fNote(7)*t + 2*pi*rand);%H

% Weigh the notes
expWtCnst = 6;
expWt = exp(-abs(expWtCnst*t));
Do = Do.*expWt;
Re = Re.*expWt;
Mi = Mi.*expWt;
Fa = Fa.*expWt;
So = So.*expWt;
La = La.*expWt;
Ti = Ti.*expWt;

% Generate note sequence
noteSequence = [Do Re Mi Fa So La Ti];

% Listen to Notes
sound(noteSequence, fSampling);

figure;
T = 0:tSampling:(7*(0.5+tSampling)-tSampling);
plot(T,noteSequence);
xlabel('time (s)')
ylabel('magnitude');

% Spectral analysis
%Parameters
nFFT = 2^14;
tuneF = fft(noteSequence);
nFFT = length(tuneF);
magTune = fftshift(abs(tuneF));
phaseTune = fftshift(unwrap(angle(tuneF)));
fSpacing = fSampling/nFFT;
fAxis = -fSampling/2:fSpacing:fSampling/2-fSpacing;

figure, plot (fAxis, 20*log10(magTune));
xlabel('Frequency F (Hz)');
ylabel('Magnitude X(F) (dB)');
axis([-2000 2000 0 20]);

figure, plot( fAxis, phaseTune);
xlabel('Frequency F (Hz)');
ylabel('Phase X(F)');

% spectrogram
figure
spectrogram(noteSequence, 256,0,[], fSampling);