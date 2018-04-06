function set_area() {
	$("#range_x_min").val(area.range.x.min/10).attr("max",area.dimension.x/10);
	$("#range_x_max").val(area.range.x.max/10).attr("max",area.dimension.x/10);
	$("#range_y_min").val(area.range.y.min/10).attr("max",area.dimension.y/10);
	$("#range_y_max").val(area.range.y.max/10).attr("max",area.dimension.y/10);
	$(".ui-rangeslider").rangeslider("refresh");
	$("#dimension_x").val(area.dimension.x/10);
	$("#dimension_y").val(area.dimension.y/10);
	$("#dimension_z").val(area.dimension.z/10);
}

function set_option() {
	$("#spool_front").val(option.spool.front/1000);
	$("#spool_back").val(option.spool.back/1000);
	$("#motorSpeedLimit").val(option.motorSpeedLimit);
	$("#gearRatio").val(option.gearRatio);
	$("#stepping").val(option.stepping);
}

function set_lineLengh() {
	$("#front-cm").html(line.current.lengh.front);
	$("#back-cm").html(line.current.lengh.back);
	$("#front-steps").html(line.current.steps.front);
	$("#back-steps").html(line.current.steps.back);
}

//$("#adminRefresh").trigger("click");  to get steps
