function set_area() {
	$("#range_x_min").val(area.range.x.min/10).attr("max",area.dimension.x/10);
	$("#range_x_max").val(area.range.x.max/10).attr("max",area.dimension.x/10);
	$("#range_y_min").val(area.range.y.min/10).attr("max",area.dimension.y/10);
	$("#range_y_max").val(area.range.y.max/10).attr("max",area.dimension.y/10);
	$(".ui-rangeslider").rangeslider("refresh");
	$("#dimension_x").val(area.dimension.x/10);
	$("#dimension_y").val(area.dimension.y/10);
	$("#dimension_z").val(area.dimension.z/10);
	$("#voltageLeft").html(parseInt(option.voltage.unit1/100000)/10+" Volt");
	$("#voltageRight").html(parseInt(option.voltage.unit2/100000)/10+" Volt");
}

function set_option() {
	$("#spool_front").val(option.spool.front/1000);
	$("#spool_back").val(option.spool.back/1000);
	$("#motorSpeedLimit").val(option.motorSpeedLimit);
	$("#gearRatio").val(option.gearRatio);
	$("#stepping").val(option.stepping);
	if (option.loaded) {
		if (option.loaded.name) {
			$("#loaded").html(option.loaded.name);
			$("#loaded").show();
		}
	} else {
		$("#loaded").hide();
	}
}

function set_lineLengh() {
	$("#front-cm").html(line.current.lengh.front);
	$("#back-cm").html(line.current.lengh.back);
	$("#front-steps").html(line.current.steps.front);
	$("#back-steps").html(line.current.steps.back);
}

function deleteSlot(e) {
	$.mobile.loading( "show", {
		text: "speichern",
		textVisible: true
	});
	var sendPackage = {};
	sendPackage.slot = e;
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		method: "POST",
		url: server.protocol+"://"+server.host+":"+server.port+"/deleteSlot",
		timeout: 5000,
		dataType: "text",
		contentType: "application/json; charset=utf-8",
		data: JSON.stringify(sendPackage)
	})
	.done(function() {
		$.mobile.loading( "hide" );
		load_memIndexList();
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "failed",
			textVisible: true,
			textonly: true
		})
	})
}

function create_deleteIndexList() {
	$("#deleteIndexList").html("");
	Object.keys(memory).forEach(function (index, element) {
		if ( index > 0 && index <= 100 ) {
			if (memory[index].name == "") memory[index].name = "- ohne Bezeichnung -"
			if (index > 9 && index <= 99) $("#deleteIndexList").append('<li><a href=# onclick="deleteSlot('+index+');">'+memory[index].name+'</a><div class="ui-grid-a"><div class="ui-block-a"><div class="ui-bar">Höhe:<br>'+parseInt((memory[index].y/10)+0.5)+' cm</div></div><div class="ui-block-b"><div class="ui-bar">Tiefe:<br>'+parseInt((memory[index].x/10)+0.5)+' cm</div></div></div></li>');
		}
	});
	$('#deleteIndexList').listview('refresh');
}

function calibrateSingle(motor,direction) {
	$.mobile.loading( "show", {
		text: "sending",
		textVisible: true
	});
	var sendPackage = {};
	sendPackage.motor  = motor;
	sendPackage.amount = $("#calibrationSlider").val() * direction
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		method: "POST",
		url: server.protocol+"://"+server.host+":"+server.port+"/calibrateSingle",
		timeout: 2000,
		dataType: "text",
		contentType: "application/json; charset=utf-8",
		data: JSON.stringify(sendPackage)
	})
	.done(function() {
		$.mobile.loading( "hide" );
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "failed",
			textVisible: true,
			textonly: true
		})
	})
}
