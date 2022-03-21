clear
clc

const = {};
const.earth_r = 6378e3; % m
const.c = 299.8e6; % m/s
const.k = -228.6; % dB

sat = {};
sat.alt = 250e3; % m
sat.r = const.earth_r + sat.alt; % m
sat.angle = 80; % max angle to 'view' satellite, deg
sat.frequency = 435.800e6; % Hz
sat.power = 2; % W
sat.gain = 0; % dBi
sat.line_loss = -0.5; % dB
sat.effective_noise = 1345; % K

gs = {};
gs.frequency = 435.800e6; % Hz
gs.power = 100; % W
gs.gain = 15.5; %dBi
gs.line_loss = -6; % dB
gs.effective_noise = 601; % K

env = {};
env.atmospheric_loss = -10; % dB
env.polarisation_loss = -1; % dB
env.antenna_alignment_loss = -1; % dB
env.Eb_N0_min = 20; % dB

data = {};
data.rate = 9600; % max bps according to AX.25

% Calc max dist of satellite to ground station, m
% find elevation
eps_0 = deg2rad(90-sat.angle);
% use sine rule to find nadir
alpha_0 = asin(const.earth_r*sin(eps_0+pi/2)/sat.r);
% use angle sum to find central angle
beta_0 = pi-alpha_0-(eps_0+pi/2);
% use cosine rule to find distance
env.dist = sqrt(sat.r^2+const.earth_r^2-2*sat.r*const.earth_r*cos(beta_0));

% Calc wavelengths
sat.wavelength = const.c/sat.frequency;
gs.wavelength = const.c/gs.frequency;

% Convert power to dbW
sat.power = 10*log10(sat.power);
gs.power = 10*log10(gs.power);

cases = ["uplink", "downlink"];

for i = 1:length(cases)
    disp("Doing case " + cases(i));
    if cases(i) == "uplink"
        transmitter = gs;
        receiver = sat;
    elseif cases(i) == "downlink"
        transmitter = sat;
        receiver = gs;
    else
        disp("Unrecognised case");
        continue
    end
    
    % Effective Isotropic Radiated Power
    EIRP = transmitter.gain + transmitter.power + transmitter.line_loss;
    
    % Free Space Path Loss
    FSPL = 10*log10(((4*pi*env.dist*transmitter.frequency)/const.c)^2);
    
    % Gain to Noise Ratio
    G_Ta = receiver.gain - 10*log10(receiver.effective_noise);
    
    % Other losses
    losses = env.atmospheric_loss + env.polarisation_loss ...
                                + env.antenna_alignment_loss;
    
    % Carrier to Noise Density Ratio
    C_N0 = EIRP + G_Ta - FSPL - const.k + losses;
    
    % Signal to Noise Ratio
    Eb_N0 = C_N0 - 10*log10(data.rate);

    % Link Margin
    margin = Eb_N0 - env.Eb_N0_min;
    
    if margin < 0
        disp("FAILURE :(")
    else
        disp("SUCCESS :)")
    end
    disp("EIRP:   " + EIRP)
    disp("FSPL:   " + FSPL)
    disp("G/Ta:   " + G_Ta)
    disp("C/N0:   " + C_N0)
    disp("Eb/N0:  " + Eb_N0)
    disp("Margin: " + margin)
    disp(" ")
end