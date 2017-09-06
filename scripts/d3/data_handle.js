var d3 = require('d3');
console.log(d3.version);
var data_file = "./data/disc20_functional_plot2.csv";
// d3.text("disc20_functional_plot2.txt")
// d3.csv(data_file, function(error, data) {
//     var headerNames = d3.keys(data[4]) ;
//     console.log(headerNames);
// });

d3.csv(data_file, function(text) {
    console.log(text);
}
);

// dataArray = [25,26,28,32,37,45,55,70,90,120,135,150,160,168,172,177,180];
// var dataYears = ['2000','2001','2002','2003','2004','2005','2006','2007','2008','2009','2010','2011','2012','2013','2014','2015','2016'];
//
// var height = 200;
// var width = 500;
//
//
// var area = d3.area()
//                 .x(function(d,i){ return i*20; })
//                 .y0(height)
//                 .y1(function(d){ return height-d; });
// var svg = d3.select("body").append("svg").attr("height","100%").attr("width","100%");
// svg.append("path").attr("d",area(dataArray));
//
