// initial

var using_file_index = -1;
var edit_file_name = "";
var move_file_name = "";

var vm_move = new Vue({
	el: "#vm_move",
	data: {
		move_list: []
	}
});

$("#b_move_move").click(function(){
	$("#movepage").css("display","none");
	$("#loadpage").css("display","flex");
	url_temp = "http://dnsset.idv.tw:4000/mv.cgi?x="+username+"&y="+password;
	for(var i=1;i<vm_level.level_list.length;i++){
		url_temp = url_temp + "&from=" + vm_level.level_list[i];
	}
	url_temp = url_temp + "&from=" + move_file_name;

	for(var i=1;i<vm_level.level_list.length;i++){
		url_temp = url_temp + "&to=" + vm_level.level_list[i];
	}
	url_temp = url_temp + "&to=" + $("#vm_move").val();
	
	$.ajax({
		url: url_temp,
		success: function(res){
			if(res.response == 1){
				refresh_location_now();
			}
			else{
				console.log("Error on #b_move_move");
			}

		},
		error: function (jqXHR, exception) {
			console.log("Status:"+jqXHR.status);
			console.log("exception:"+exception);
		}
	});
});

$("#b_move_cancel").click(function(){
	$("#movepage").css("display","none");
});

var vm_search = new Vue({
	el: "#vm_search",
	data: {
		search_result_list: []
	},
	ready: function(){
		this.search_result_list = [];
	},
	watch: {
		search_result_list: function(){
			$(".td_search_result").each(function(index){
				$(this).click(function(){
					$(".td_search_result").css("background-color","white");
					$(this).css("background-color","#e8f1ff");
				}).dblclick(function(e){
					$("#searchpage").css("display","none");
					$("#loadpage").css("display","flex");
					vm_level.level_list = vm_search.search_result_list[index].dir_cut;
					refresh_location_now();
				});
			});
		}
	}
});

$("#b_search_search").click(function(){

	if($("#search_name").val() == ""){
		$("#search_result_information").text("Input is empty");
		$("#search_result_information").css("border","solid 2px red");
		$("#search_result_information").css("color","red");
		$("#search_result_information").css("display","inline-block");

		setTimeout(function(){
			$("#search_result_information").css("display","none");
		},1300);
	}
	else{
		url_temp = "http://dnsset.idv.tw:4000/search.cgi?x="+username+"&y="+password+"&search="+$("#search_name").val();

		$("#search_result_information").text("Searching ...");
		$("#search_result_information").css("border","solid 2px black");
		$("#search_result_information").css("color","black");
		$("#search_result_information").css("display","inline-block");

		$.ajax({
			url: url_temp,
			success: function(res){
				$("#search_result_information").css("border","solid 2px blue");
				$("#search_result_information").css("color","blue");
				$("#search_result_information").text("Searching done");
				vm_search.search_result_list = res.response;
				setTimeout(function(){
					$("#search_result_information").css("display","none");
				},1300);
			},
			error: function (jqXHR, exception) {
				console.log("Status:"+jqXHR.status);
				console.log("exception:"+exception);
			}
		});
	}
});

$("#b_search_cancel").click(function(){
	$("#searchpage").css("display","none");
});


var vm_level = new Vue({
	el: "#location",
	data: {
		level_list: []
	},
	ready: function(){
		this.level_list.push(username);
	},
	watch: {
		level_list: function(){
			$(".level_name").each(function(index){
				$(this).click(function(){
					$("#loadpage").css("display","flex");

					// remove from list
					var delete_count = vm_level.level_list.length - index - 1;
					for(var i=0;i<delete_count;i++){
						vm_level.level_list.pop();
					}

					refresh_location_now();
				});
			});
		}
	}
});

var vm0 = new Vue({
	el: "#vm0",
	data: {
		new_file: 0,
		new_folder: 0
	}
});

var vm1=new Vue({
	el: "#vm1",
	data: {
		file_list : []
	},
	ready: function(){
		refresh_location_now();
	},
	watch: {
		file_list: function(){
			$(".tr_line").each(function(index){
				$(this).click(function(){
					$(".tr_line").css("background-color","white");
					$(this).css("background-color","#e8f1ff");
				}).dblclick(function(){
					if(vm1.file_list[index].filetype == 1){
						$("#show_file_name").text(vm1.file_list[index].filename);
						$("#loadpage").css("display","flex");

						url_temp = "http://dnsset.idv.tw:4000/cat.cgi?x="+username+"&y="+password+"&mode=2";
						for(var i=1;i<vm_level.level_list.length;i++){
							url_temp = url_temp + "&cd=" + vm_level.level_list[i];
						}
						url_temp = url_temp + "&cd=" + vm1.file_list[index].filename;
						
						$.ajax({
							url: url_temp,
							success: function(res){
								console.log(res);
								var regex = new RegExp("%20", "g");
								ttemp = res.replace(regex, " ");
								$("#content").html(ttemp);
								$("#loadpage").css("display","none");
								$("#showpage").css("display","flex");
							},
							error: function (jqXHR, exception) {
								console.log("Status:"+jqXHR.status);
								console.log("exception:"+exception);
							}
						});
					}
					else{
						$("#loadpage").css("display","flex");
						vm_level.level_list.push(vm1.file_list[index].filename);
						url_temp = "http://dnsset.idv.tw:4000/ls.cgi?x="+username+"&y="+password;
						for(var i=1;i<vm_level.level_list.length;i++){
							url_temp = url_temp + "&cd=" + vm_level.level_list[i];
						}
						$.ajax({
							url: url_temp,
							success: function(res){
								vm1.file_list = res.response;
								$("#loadpage").css("display","none");
							},
							error: function (jqXHR, exception) {
								console.log("Status:"+jqXHR.status);
								console.log("exception:"+exception);
							}
						});
					}
				});
			});
			
			$(".b_move").each(function(index,value){
				$(this).click(function(){
					move_file_name = vm1.file_list[index].filename;
					$("#move_title").text("Move \""+vm1.file_list[index].filename+"\"");
					$("#movepage").css("display","flex");

					var move_temp_list = [];
					if(vm_level.level_list.length != 1){
						move_temp_list = [".."];
					}
					for(var i=0;i<vm1.file_list.length;i++){
						if(index != i && vm1.file_list[i].filetype == 0){
							move_temp_list.push(vm1.file_list[i].filename);
						}
					}

					vm_move.move_list = move_temp_list;
				});
			});

			$(".b_edit").each(function(index,value){
				$(this).click(function(){
					var i=0;
					var is_file_count = 0;
					for(i=0;i<vm1.file_list.length;i++){
						if(vm1.file_list[i].filetype == 1 && is_file_count == index){
							break;
						}
						else if(vm1.file_list[i].filetype == 1){
							is_file_count++;
						}
					}
					$("#loadpage").css("display","flex");

					//title
					$("#edit_file_name").text(vm1.file_list[i].filename);
					edit_file_name = vm1.file_list[i].filename;
					//content
					url_temp = "http://dnsset.idv.tw:4000/cat.cgi?x="+username+"&y="+password+"&mode=1";
					for(var j=1;j<vm_level.level_list.length;j++){
						url_temp = url_temp + "&cd=" + vm_level.level_list[j];
					}
					url_temp = url_temp + "&cd="+vm1.file_list[i].filename;
					$.ajax({
						url: url_temp,
						success: function(res){
							$("#textarea").text(res);
							$("#loadpage").css("display","none");
							$("#editpage").css("display","flex");
						},
						error: function (jqXHR, exception) {
							console.log("Status:"+jqXHR.status);
							console.log("exception:"+exception);
						}
					});
				}).dblclick(function(e){
					e.stopPropagation();
				});
			});

			$(".b_recover").each(function(index,value){
				$(this).click(function(){
					$("#loadpage").css("display","flex");

					url_temp = "http://dnsset.idv.tw:4000/recover.cgi?x="+username+"&y="+password;
					url_temp = url_temp + "&file="+ vm1.file_list[index].filename;

					$.ajax({
						url: url_temp,
						success: function(res){
							refresh_location_now();
							$("#loadpage").css("display","none");
						},
						error: function (jqXHR, exception) {
							console.log("Status:"+jqXHR.status);
							console.log("exception:"+exception);
						}
					});					
				});
			});

			$(".b_del").each(function(index){
				$(this).click(function(){
					var answer = confirm("Are you sure? (Delete \""+vm1.file_list[index].filename+"\")")
					if (answer) {
						$("#loadpage").css("display","flex");

						url_temp = "http://dnsset.idv.tw:4000/rm.cgi?x="+username+"&y="+password;
						for(var j=1;j<vm_level.level_list.length;j++){
							url_temp = url_temp + "&cd=" + vm_level.level_list[j];
						}
						url_temp = url_temp + "&cd="+vm1.file_list[index].filename;
						$.ajax({
							url: url_temp,
							success: function(res){
								if(res.response == 1){
									refresh_location_now();
									$("#loadpage").css("display","none");
								}
								else{

								}
							},
							error: function (jqXHR, exception) {
								console.log("Status:"+jqXHR.status);
								console.log("exception:"+exception);
							}
						});
					}
				});
			});

		}
	}
});

$("#b_home").click(function(){
	$("#loadpage").css("display","flex");

	vm_level.level_list = [username];
	
	refresh_location_now();
});

$("#b_trash").click(function(){
	$("#loadpage").css("display","flex");

	vm_level.level_list = [username,".Trash"];

	refresh_location_now();
});

$("#b_search").click(function(){
	vm_search.search_result_list = [];
	$("#searchpage").css("display","flex");
	$("#search_name").val("");
});


$("#b_add_file").click(function(){
	$("#vm0").css("display","block");
	$("#add_name").val("");
	vm0.new_file = 1;
	vm0.new_folder = 0;
});

$("#b_add_folder").click(function(){
	$("#vm0").css("display","block");
	$("#add_name").val("");
	vm0.new_file = 0;
	vm0.new_folder = 1;
});

$("#b_back").click(function(){
	$("#showpage").css("display","none");
});

$("#b_save").click(function(){

	$("#editpage").css("display","none");
	$("#loadpage").css("display","flex");
	// make url
	url_temp = "http://dnsset.idv.tw:4000/echo.cgi?x="+username+"&y="+password+"&new1=" + $("#textarea").val();
	for(var i=1;i<vm_level.level_list.length;i++){
		url_temp = url_temp + "&new=" + vm_level.level_list[i];
	}
	url_temp = url_temp + "&neww=" + edit_file_name;
	
	$.ajax({
		url: url_temp,
		success: function(res){
			if(res.response == 1){
				$("#loadpage").css("display","none");
			}
			else{
				console.log("Error!");
			}
		},
		error: function (jqXHR, exception) {
			console.log("Status:"+jqXHR.status);
			console.log("exception:"+exception);
		}
	});

});

$("#b_edit_back").click(function(){
	$("#editpage").css("display","none");
});


$("#b_function_add").click(function(){

	if( $("#add_name").val() == ""){
		$("#vm0").css("display","none");
		$("#result_information").text("Name is empty");
		$("#result_information").css("display","inline-block");
		$("#result_information").css("color","red");
		$("#result_information").css("border","solid 1.5px red");
		
		setTimeout(function(){
			$("#result_information").css("display","none");
		},1500);
	}
	else if(vm0.new_file == 1){
		
		$("#loadpage").css("display","flex");

		// make url
		url_temp = "http://dnsset.idv.tw:4000/touch.cgi?x="+username+"&y="+password;
		for(var i=1;i<vm_level.level_list.length;i++){
			url_temp = url_temp + "&new=" + vm_level.level_list[i];
		}
		url_temp = url_temp + "&new=" + $("#add_name").val();

		$.ajax({
			url: url_temp,
			success: function(res){
				if(res.response == 1){
					$("#loadpage").css("display","none");
					$("#vm0").css("display","none");
					$("#result_information").text("Add File Success");
					$("#result_information").css("display","inline-block");
					$("#result_information").css("color","#28a745");
					$("#result_information").css("border","solid 1.5px #28a745");
					
					setTimeout(function(){
						$("#result_information").css("display","none");
					},1500);

					refresh_location_now();
				}
				else if(res.response == 0){
					$("#vm0").css("display","none");
					$("#result_information").text("Add File Failed");
					$("#result_information").css("display","inline-block");
					$("#result_information").css("color","red");
					$("#result_information").css("border","solid 1.5px red");
					
					setTimeout(function(){
						$("#result_information").css("display","none");
					},1500);
				}
			},
			error: function (jqXHR, exception) {
				console.log("Status:"+jqXHR.status);
				console.log("exception:"+exception);
			}
		});
	}
	else if(vm0.new_folder == 1){
		$("#loadpage").css("display","flex");

		// make url
		url_temp = "http://dnsset.idv.tw:4000/mkdir.cgi?x="+username+"&y="+password;
		for(var i=1;i<vm_level.level_list.length;i++){
			url_temp = url_temp + "&mk=" + vm_level.level_list[i];
		}
		url_temp = url_temp + "&mk=" + $("#add_name").val();
		
		// add folder
		$.ajax({
			url: url_temp,
			success: function(res){
				if(res.response == 1){
					$("#loadpage").css("display","none");
					$("#vm0").css("display","none");
					$("#result_information").text("Add Folder Success");
					$("#result_information").css("display","inline-block");
					$("#result_information").css("color","#28a745");
					$("#result_information").css("border","solid 1.5px #28a745");
					
					setTimeout(function(){
						$("#result_information").css("display","none");
					},1500);

					refresh_location_now();
				}
				else if(res.response == 0){
					$("#vm0").css("display","none");
					$("#result_information").text("Add Folder Failed");
					$("#result_information").css("display","inline-block");
					$("#result_information").css("color","red");
					$("#result_information").css("border","solid 1.5px red");
					
					setTimeout(function(){
						$("#result_information").css("display","none");
					},1500);
				}
			},
			error: function (jqXHR, exception) {
				console.log("Status:"+jqXHR.status);
				console.log("exception:"+exception);
			}
		});
	}
});


$("#b_function_cancel").click(function(){
	$("#vm0").css("display","none");
	vm0.new_file = 0;
	vm0.new_folder = 0;
});

function refresh_location_now(){
	url_temp1 = "http://dnsset.idv.tw:4000/space.cgi?x="+username+"&y="+password;
	$.ajax({
		url: url_temp1,
		success: function(res){
			$("#username").text(res.response);
		},
		error: function (jqXHR, exception) {
			console.log("Status:"+jqXHR.status);
			console.log("exception:"+exception);
		}
	});


	url_temp = "http://dnsset.idv.tw:4000/ls.cgi?x="+username+"&y="+password;
	for(var i=1;i<vm_level.level_list.length;i++){
		url_temp = url_temp + "&cd=" + vm_level.level_list[i];
	}
	$.ajax({
		url: url_temp,
		success: function(res){
			vm1.file_list = res.response;
			$("#loadpage").css("display","none");
		},
		error: function (jqXHR, exception) {
			console.log("Status:"+jqXHR.status);
			console.log("exception:"+exception);
		}
	});
}

// $("#editpage").css("display","flex");
// $("#showpage").css("display","flex");
// $("#movepage").css("display","flex");
// $("#searchpage").css("display","flex");
