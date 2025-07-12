clear all
close all
clc

Kp=1.4;
Ki=0.015;

tsim=100;
sim('pidunion.slx')

t=ans.fermi(:,1);
y=ans.fermi(:,2);
R=ans.fermi(:,3);
u=ans.fermi(:,4);

plot(t,y,t,R,t,u)
legend('salida y','referencia','Entrada U')
grid on