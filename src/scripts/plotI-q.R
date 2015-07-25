#!/usr/bin/env Rscript

args = commandArgs(trailingOnly=TRUE);
#Default setting when no argument:
if(length(args) < 1){
  args <- c("--help")
}
# Help argument:
helpString = '
      Plot I vs q
      
      Arguments:
      --input=/path/to/file     - Path to input I-q plot file.
             File must be 2 columns with a header:
             #input_file=xxxx
             #Nx=x
             #Ny=y
      --nm_per_pixel=1.0        - Float. Image Resolution, Nanometers per pixel
      
      --output=/path/to/out.svg - File output.Accepts:
      --filetype=svg            - Output format
                                    (*) svg
                                        tikz
                                        pdf

      Example:
      ./plotI-q --input=./data.plot --nm_per_pixel=0.82

      Dependencies:
      install.packages("ggplot2")
      \n\n'
if ("--help" %in% args) {
  cat(helpString)
  q(save="no")
}
library(tools)
#Parse (--arg=value)
parseArgs <- function(x) strsplit(sub("^--", "", x), "=")
argsDF <- as.data.frame(do.call("rbind",
                                parseArgs(args)))
argsL <- as.list(as.character(argsDF$V2))
names(argsL) <- argsDF$V1
# print(argsL)
## Required arguments:
if(is.null(argsL$input))
{
  cat(helpString)
  q(save="no")
}
filename = argsL$input;
cat(paste("--input=",filename, "\n", sep=""));

if(is.null(argsL$nm_per_pixel)){

    argsL$nm_per_pixel=1.0
    cat(paste("--nm_per_pixel NOT set, default to", argsL$nm_per_pixel,"\n"))
}

if(is.null(argsL$filetype)){

    argsL$filetype="svg"
    cat(paste("--filetype NOT set, default to", argsL$filetype, "\n"))
}
cat(paste("--filetype=",argsL$filetype, "\n", sep=""));

if(is.null(argsL$output)){
    argsL$output= paste(file_path_sans_ext(argsL$input), argsL$filetype, sep=".")
    cat(paste("--output NOT set, default to", argsL$output,"\n"))
}
# If output is defined but has no extension, append it.
if(file_ext(argsL$output) == "" ) {
    argsL$output = paste(argsL$output,".", argsL$filetype, sep="");
} else if( file_ext(argsL$output) != argsL$filetype){
    argsL$filetype = file_ext(argsL$output)
    cat("WARNING: filetype and output file has different extensions. Changed filetype to follow hardtyped output extension\n")
    cat(paste("--filetype=",argsL$filetype, "\n", sep=""));
}

cat(paste("--output=",argsL$output, "\n", sep=""));

#Read data.
data = read.table(filename, col.names=c("d", "I"), row.names=NULL);
#Read header.
header = scan(file(filename), what="character", nlines=3)
fname = unlist(strsplit(header[2], "="))[2];
Nx = as.numeric(unlist(strsplit(header[4], "="))[2]);
Ny = as.numeric(unlist(strsplit(header[6], "="))[2]);
cat(paste("Nx =", Nx," ", "Ny =", Ny, "\n"))
nm_per_pixel = as.numeric(argsL$nm_per_pixel);
cat(paste("nm_per_pixel =",nm_per_pixel, "\n"));

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

# if(q[1] == 0) I[1]=I[2]*10 # for correct I plot limits
datf = data.frame(q,I);

# #ADD SAXS DATA
# fileSaxs = "/home/phc/Dropbox/Shared-Geelong-Palmerston/pectin/Pectin1_acid/SAXS_1car200NaCl10A_1237_longMod.txt"
# dataS = read.table(fileSaxs, col.names=c("d", "I"), row.names=NULL, skip=1);
# qS = dataS[,"d"];
# IS = dataS[,"I"]*10^11;
# datafS = data.frame(qS,IS);

library("ggplot2")
library("scales")
filenameNoExtension = basename(file_path_sans_ext(filename));
p <-ggplot()+
    theme_bw() +
    # remove minor ticks
    theme(panel.grid.minor = element_blank()) +
    geom_line(data = datf, aes(x=q, y=I)) +
    # geom_line(data = datafS, colour="blue", aes(x = qS, y = IS)) +
    labs(title=filenameNoExtension, x=" q $nm^{-1}$", y="I") +
    scale_y_log10(breaks = trans_breaks("log10", function(x) 10^x, n=4),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    scale_x_log10(breaks = trans_breaks("log10", function(x) 10^x, n=3),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    annotation_logticks(SCALED=FALSE)

#####tikzDevice
# library("tikzDevice")
# options(tikzDefaultEngine = 'luatex')
# # options(tikzDefaultEngine = 'pdftex')
# options(tikzLatex=paste(latexroot,"pdflatex", sep=''));
# options(tikzLualatex=paste(latexroot,"lualatex",sep=''));
# options(tikzXelatex=paste(latexroot,"xelatex", sep=''));
###############
motherdirOutput = dirname(argsL$output)
createDirOuput = dir.create(motherdirOutput, recursive=TRUE)
if(argsL$filetype == "svg") {
    svg(filename=argsL$output, width=6,height=6,pointsize=10)
    print(p)
    dev.off();
} else if (argsL$filetype == "pdf"){
    pdf(file=argsL$output)
    print(p)
    dev.off();
} else if (argsL$filetype == "tikz"){
    library("tikzDevice")
    tikz(file=argsL$output, standAlone = TRUE)
    print(p)
    dev.off();
} else {
    cat(paste("WARNING: Unrecognised filetype option --filetype=",argsL$filetype,"\n","No output file generated.\n", sep=""));
}

