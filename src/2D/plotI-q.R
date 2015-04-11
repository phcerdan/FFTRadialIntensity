#!/usr/bin/env Rscript

filename = commandArgs(trailingOnly=TRUE)[1];
print(paste("filename :",filename));
data = read.table(filename, col.names=c("d", "I"), row.names=NULL);
# print(data[,"d"]);
header = scan(file(filename), what="character", nlines=3)
# HEADER format:
# #input_file=thefilename
# #Nx=x
# #Ny=y
fname = unlist(strsplit(header[2], "="))[2];
Nx = as.numeric(unlist(strsplit(header[4], "="))[2]);
Ny = as.numeric(unlist(strsplit(header[6], "="))[2]);
print(paste("Nx =", Nx," ", "Ny =", Ny))

nm_per_pixel = commandArgs(trailingOnly=TRUE)[2];
print(paste("nm_per_pixel =",nm_per_pixel));
nm_per_pixel = as.numeric(nm_per_pixel);

dx = nm_per_pixel;
dy = nm_per_pixel;
dfx = 1.0/(dx);
dfy = 1.0/(dy);
df = min(dfx, dfy);
# df = sqrt(dfx*dfx + dfy*dfy);
# Nsize = sqrt(Nx^2 + Ny^2)
# df = (1.0/Nsize) * (1.0/dx);
Nmin = min(Nx,Ny);
dmax = as.integer(Nmin/2)
q = data[,"d"] * df / Nmin;
I = data[,"I"];

# q_trim = q[1:dmax]
# I_trim = I[1:dmax]
# plot(q_trim, I_trim, log="xy");


plot(q, I, log="xy");
lines(q, I)
title(fname)
axis(1, q, outer=TRUE, xpd=TRUE)

library("ggplot2")
library("scales")
library(tools)
# pdfName = paste(file_path_sans_ext(filename), ".pdf", sep='');
# pdf(file=pdfName)
library("tikzDevice")
tikzNameO = file_path_sans_ext(filename);
tikzNameO = unlist(strsplit(tikzNameO, "_"));
tikzName = paste(paste(head(tikzNameO ), collapse=""), ".tex", sep='');
print(tikzName)
tikz(file=tikzName, standAlone = TRUE)
if(q[1] == 0) I[1]=I[2]*10 # for correct I plot limits
datf = data.frame(q,I);
#ADD SAXS DATA
dataS = read.table("/home/phc/Dropbox/Shared-Geelong-Palmerston/pectin/Pectin1_acid/SAXS_1car200NaCl10A_1237_longMod.txt", col.names=c("d", "I"), row.names=NULL, skip=1);
qS = dataS[,"d"];
IS = dataS[,"I"];
datafS = data.frame(qS,IS);
p <-ggplot() +  #data = datf, aes(x = q, y = I)) + theme_bw() + # white background
    theme(panel.grid.minor = element_blank()) + # remove minor ticks
    geom_line(data = datf, aes(x = q, y = I)) +
    geom_line(data = datafS, colour="blue", aes(x = qS, y = IS)) + 
    labs(title = tikzName, x=expression("q [" ~ nm^{-1} ~"]"), y=expression("I")) +
    scale_y_log10(breaks = trans_breaks("log10", function(x) 10^x, n=4),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    scale_x_log10(breaks = trans_breaks("log10", function(x) 10^x, n=3),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    annotation_logticks(SCALED=FALSE) +
#### DRAW SEGMENTS ### 
    # slope = log(yend/y)/log(xend/x)
    # geom_segment( aes(x=10^-2, y=10^7, xend=10^-0, yend=10^5)) +  # slope=-1
    # geom_text(aes(x=10^-1.4, y=10^4.5 ), label='a = -1', angle = atan2(-1,1) * 180/pi )  +
    geom_segment( aes(x=10^-2, y=10^8, xend=10^-1, yend=10^6))  # slope=-2
    # geom_text(aes(x=10^-0.4, y=10^4 ), label='a = -2', angle = atan2(-2,1) * 180/pi ) +
    # geom_segment( aes(x=10^-1, y=10^6, xend=10^0, yend=10^3))  # slope=-3
    # geom_text(aes(x=10^-0.4, y=10^5 ), label='a = -3', angle = atan2(-3,1) * 180/pi )


#Add new data from SAXS.


p
dev.off(); # To switch off plot.
# print(paste("Output pdf file generated:", pdfName));
