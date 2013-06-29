function outputParams=LandmarkRegistration(inputParams)
% Example function that computes a rigid similarity transform between two sets of points

fixedPoints=cli_pointvectordecode(inputParams.fixedpoints);
movingPoints=cli_pointvectordecode(inputParams.movingpoints);
doScale=isfield(inputParams,'enablescaling');

[regParams,Bfit,ErrorStats]=absor(fixedPoints,movingPoints,'doScale',doScale);

outputParams.lsqerror=ErrorStats.errlsq;
outputParams.maxerror=ErrorStats.errmax;

cli_lineartransformwrite(inputParams.movingtofixedtransform,regParams.M);
