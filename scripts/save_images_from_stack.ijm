//Nicholas M. Carducci, 5/24/12

//requires("1.33s");

dir = getDirectory("Choose a Directory");
setBatchMode(true);
count = 0;
countFiles(dir);
n = 0;
processFiles(dir);

function countFiles(dir) {
    list = getFileList(dir);
    for (i=0; i<list.length; i++) {
        if (endsWith(list[i], "/"))
            countFiles(""+dir+list[i]);
        else
            count++;
    }
}

function processFiles(dir) {
    list = getFileList(dir);
    for (i=0; i<list.length; i++) {
        if (endsWith(list[i], "/"))
            processFiles(""+dir+list[i]);
        else {
            showProgress(n++, count);
            path = dir+list[i];
            processFile(path);
        }
    }
}
function processFile(path) {
    if (endsWith(path, ".tif")) {
        open(path);
        title=getTitle();
        len = lengthOf(title);
        lenPath = lengthOf(path);
        path = substring(path, 0, lenPath-len);
        for(i=1;i<=95; i++){
          newTitle = substring(title, 0, len-4)+"_Slice_"+i;
          newPath = path+newTitle;
          run("Select All");
          setSlice(i);
          run("Copy");
          newImage("Untitled", "8-bit White", 1881, 1881, 1);
          run("Paste");
          saveAs("Tiff", newPath);
          close();
        }
      close();
    }
}
