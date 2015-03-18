#!/usr/bin/Rscript

filename = commandArgs(trailingOnly=TRUE)[1];
print(filename);
data = read.table(filename, col.names=c("d", "I"), row.names=NULL);
# print(data[,"d"]);

nm_per_pixel = commandArgs(trailingOnly=TRUE)[2];
nm_per_pixel = 0.72
print(nm_per_pixel)
nm_per_pixel = as.numeric(nm_per_pixel);

dx = nm_per_pixel;
dy = nm_per_pixel;
Nx = 6144
Ny = 6144
dfx = 1.0/(Nx*dx);
dfy = 1.0/(Ny*dy);
df = sqrt(dfx*dfx + dfy*dfy);

dd = sqrt(dx *dx + dy*dy)
Nd = sqrt(Nx *Nx + Ny*Ny)
dfd = 1.0/(Nd*dd)

q = data[,"d"] * dfd;
I = data[,"I"];
q = q - 0.499*dfd
pdf(file=paste(filename, ".pdf", sep=''))
plot(q, I, log=c("x", "y"));
title("I-q")
axis(1, q, outer=TRUE, xpd=TRUE)


