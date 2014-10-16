function outputParams=MeshScale(inputParams)
% Example function that changes scales of a surface mesh

mesh = cli_geometryread(inputParams.inputmesh);

mesh.vertices = mesh.vertices * diag([inputParams.scalex inputParams.scaley inputParams.scalez]);

cli_geometrywrite(inputParams.outputmesh, mesh);
