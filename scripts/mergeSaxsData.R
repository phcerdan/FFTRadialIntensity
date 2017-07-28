#!/usr/bin/env Rscript

# Merge Long and Short SAXS data into one data.frame with smooth intensities.
# In the merge, short data has preference because is less noisy, except in the very beggining.
# You can tune the number of starting points you want to remove from the Short data, with removeIndexShort
# To use this function, use source ('/path/to/mergeSaxsData.R')
# Return a data.frame:
# dmerged = mergeSaxs(long, short, 5 );
# you can use write.table(dmerged, file=./mergedResult.txt) if you want to save the data.

mergeSaxs <- function(fileSaxsLong, fileSaxsShort, removeIndexShort = 5, skipLong = 2, skipShort = 2, numcolsLong = 3, numcolsShort = 3) {
    # fileSaxsLong = "/home/phc/Dropbox/Shared-Geelong-Palmerston/Carrageenan/Carrageenan_Na/1car300NaCl10A_1254_long.dat"
    if (numcolsLong==3) {
        dSaxsLong = read.table(fileSaxsLong, col.names=c("d", "I", "err"), row.names=NULL, skip=skipLong);
    } else {
        dSaxsLong = read.table(fileSaxsLong, col.names=c("d", "I"), row.names=NULL, skip=skipLong);
    }
    qSL = dSaxsLong[,"d"];
    ISL = dSaxsLong[,"I"];
    dfSL = data.frame(qSL,ISL);
    # fileSaxsShort = "/home/phc/Dropbox/Shared-Geelong-Palmerston/Carrageenan/Carrageenan_Na/1car300NaCl10A_1238_short.dat"
    if (numcolsShort==3) {
        dSaxsShort = read.table(fileSaxsShort, col.names=c("d", "I", "err"), row.names=NULL, skip=skipShort);
    } else {
        dSaxsShort = read.table(fileSaxsShort, col.names=c("d", "I"), row.names=NULL, skip=skipShort);
    }
    qSS = dSaxsShort[,"d"];
    ISS = dSaxsShort[,"I"];
    dfSS = data.frame(qSS,ISS);
    # Merge both saxs data:
    trimindex = as.integer(removeIndexShort);
    # trimindex = 10;
    qs1 = qSS[trimindex];
    # Get the the closest values of qs1
    eps = 0.01
    qL_range = qSL[ qSL < qs1 + eps * qs1 & qSL > qs1 - eps * qs1];
    # Trim qL_range to 2 values:
    qL_range = c(qL_range[1], tail(qL_range, n=1));
    # Get the Intensity of those two values (probably worth to work with data.frames instead of vectors)
    m1 = match(qL_range[1], qSL);
    m2 = match(qL_range[2], qSL);
    IL_range = c(ISL[m1], ISL[m2]);
    # Get the slope to interpolate the new intensity value
    slope = (IL_range[2] - IL_range[1])/(qL_range[2] - qL_range[1]);
    #Extrapolate value
    ISextra1 = IL_range[1] + slope * (qs1 - qL_range[1]);
    Ilogdiff = log(ISS[trimindex]) - log(ISextra1);
    dSextra = subset(dfSS, subset=dfSS$qSS > qs1);
    names(dSextra) = c("q", "I");
    dSextra$I = log(dSextra$I) - Ilogdiff;
    dSextra$I = exp(dSextra$I);
    # merge the data
    dLtrim = subset(dfSL, subset=dfSL$qSL<dSextra$q[1]);
    names(dLtrim) = c("q", "I");
    dmerged = rbind(dLtrim, dSextra);

    # library("ggplot2")
    # library("scales")
    # pmerge  <- ggplot()+
    #     theme_bw() +
    #     geom_point(data = dmerged, colour="blue", aes(x = dmerged$q, y = dmerged$I)) +
    #     scale_y_log10(breaks = trans_breaks("log10", function(x) 10^x, n=4),
    #                      labels = trans_format("log10", math_format(10^.x))
    #                      ) +
    #     scale_x_log10(breaks = trans_breaks("log10", function(x) 10^x, n=3),
    #                      labels = trans_format("log10", math_format(10^.x))
    #                      ) +
    #     annotation_logticks(SCALED=FALSE)
    # pmerge
    dmerged;
}
