#!/usr/bin/env Rscript

# filename = commandArgs(trailingOnly=TRUE)[1];
filename =  "./pectinHomogeneous.plot"
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

# nm_per_pixel = commandArgs(trailingOnly=TRUE)[2];
nm_per_pixel = 0.86;
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

library(tools)
# if(q[1] == 0) I[1]=I[2]*10 # for correct I plot limits
datf = data.frame(q,I);
datf_trim = subset(datf, subset=datf$q > 10^-2.1);
#ADD SAXS DATA
# fileSaxs = "/home/phc/Dropbox/Shared-Geelong-Palmerston/pectin/Pectin1_acid/SAXS_acid_gel_good.txt"
# dataS = read.table(fileSaxs, col.names=c("d", "I"), row.names=NULL, skip=0);
fileSaxs = "/home/phc/Dropbox/Shared-Geelong-Palmerston/pectin/Pectin1_acid/Acid_gel_Aus_data.txt"
dataS = read.table(fileSaxs, col.names=c("d", "I"), row.names=NULL, skip=1);
qS = dataS[,"d"];
IS = dataS[,"I"];
dmerged = data.frame(qS,IS);
names(dmerged) = c("q", "I");
dmerged$I = dmerged$I * 10^12;
dmerged$q = dmerged$q * 10; # Change from A to nm
# Chop large q data:
# dmerged = subset(dmerged, subset=dmerged$q < 10^+0.2);
library("ggplot2")
library("scales")
motherdir = dirname(filename);
filenameNoExtension = basename(file_path_sans_ext(filename));
eps = 0.02;
# qbad = 10^-0.92;
qbad = 10^-1.33;
dbad = subset(dmerged, subset= dmerged$q < qbad + eps * qbad & dmerged$q > qbad - eps * qbad);
p <-ggplot()+
    theme_bw() +
    # remove minor ticks
    theme(panel.grid.minor = element_blank()) +
    # geom_line(data = datf_trim, aes(x=q, y=I)) +
    geom_point(data = datf_trim, aes(x=datf_trim$q, y=datf_trim$I), size=1) +
    geom_line(data = dmerged, colour="blue", aes(x = dmerged$q, y = dmerged$I)) +
    labs(title=filenameNoExtension, x=" q $nm^{-1}$", y="I") +
    scale_y_log10(breaks = trans_breaks("log10", function(x) 10^x, n=4),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    scale_x_log10(breaks = trans_breaks("log10", function(x) 10^x, n=3),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    annotation_logticks(SCALED=FALSE)
#### DRAW SEGMENTS ### 
    # slope = log(yend/y)/log(xend/x)
    p <- p + geom_segment( aes(x=10^-2, y=10^11, xend=10^-0, yend=10^9))  # slope=-1
    # geom_text(aes(x=10^-1.4, y=10^4.5 ), label='a = -1', angle = atan2(-1,1) * 180/pi )  +
# p  <- p + geom_segment( aes(x=10^-2, y=10^12, xend=10^-1, yend=10^10))  # slope=-2
    # geom_text(aes(x=10^-0.4, y=10^4 ), label='a = -2', angle = atan2(-2,1) * 180/pi ) +
    # geom_segment( aes(x=10^-1, y=10^6, xend=10^0, yend=10^3))  # slope=-3
    # geom_text(aes(x=10^-0.4, y=10^5 ), label='a = -3', angle = atan2(-3,1) * 180/pi )
## Region of interest ##
# p + geom_segment(aes(x = dbad$q, y = 0, xend = dbad$q, yend = dbad$I), colour="#CC6666");
p <- p +
    geom_vline(aes(linetype="CutOff", xintercept=dbad$q), data = dbad,  colour="#CC6666", show_guide=F) +
    geom_rect(aes(xmin = dbad$q, xmax = Inf, ymin = 0, ymax = Inf), fill="#CC6666", alpha=0.15)
p

#####tikzDevice
library("tikzDevice")
# options(tikzDefaultEngine = 'luatex')
# # options(tikzDefaultEngine = 'pdftex')
# options(tikzLatex=paste(latexroot,"pdflatex", sep=''));
# options(tikzLualatex=paste(latexroot,"lualatex",sep=''));
# options(tikzXelatex=paste(latexroot,"xelatex", sep=''));
###############
tikzdir_bool = dir.create(file.path(motherdir,"tikz"));
filenameTex = paste(filenameNoExtension, ".tex", sep='');
tikzName = file.path(motherdir,"tikz", filenameTex);
print(tikzName)
tikz(file=tikzName, standAlone = TRUE)
p
dev.off(); # To switch off plot.
# print(paste("Output pdf file generated:", pdfName));
