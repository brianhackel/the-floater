document.addEventListener('DOMContentLoaded', function () {
  const gaugeOptions = {
    chart: {
        type: 'solidgauge'
    },

    responsive: {
      rules: [{
        condition: {
          maxWidth: 530
        },
        chartOptions: {
          yAxis: [{
            title: {
              y: 43,
              style: {
                fontSize: "1em"
              }
            },
            labels: {
              enabled: false
            }
          },{
            title: {
              y: 43,
              style: {
                fontSize: "1em"
              }
            },
            labels: {
              enabled: false
            }
          },{
            title: {
              y: 43,
              style: {
                fontSize: "1em"
              }
            },
            labels: {
              enabled: false
            }
          }]
        }
      }]
    },

    title: null,
    pane: [{
      center: ['16.66%', '85%'],
      size: '140%',
      startAngle: -90,
      endAngle: 90,
      background: {
          backgroundColor:
              Highcharts.defaultOptions.legend.backgroundColor || '#EEE',
          innerRadius: '60%',
          outerRadius: '100%',
          shape: 'arc'
      }
    }, {
      center: ['50%', '85%'],
      size: '140%',
      startAngle: -90,
      endAngle: 90,
      background: {
          backgroundColor:
              Highcharts.defaultOptions.legend.backgroundColor || '#EEE',
          innerRadius: '60%',
          outerRadius: '100%',
          shape: 'arc'
      }
    }, {
      center: ['83.26%', '85%'],
      size: '140%',
      startAngle: -90,
      endAngle: 90,
      background: {
        backgroundColor:
            Highcharts.defaultOptions.legend.backgroundColor || '#EEE',
        innerRadius: '60%',
        outerRadius: '100%',
        shape: 'arc'
      }
    }],

    credits: {
        enabled: false
    },

    // the value axis
    yAxis: [{
      pane: 0,
      lineWidth: 0,
      tickWidth: 0,
      minorTickInterval: null,
      tickAmount: 2,
      title: {
        text: 'Tilt',
        y: 63,
        style: {
          fontSize: "1.3em"
        }
      },
      labels: {
        enabled: true,
        distance: '90%',
        y: 16
      },
      min: 0,
      max: 90,
      tickPositions: [0, 90],
      stops: [
        [0.1, '#ffff00'], // yellow
        [0.5, '#ff8c00'], // orange
        [0.9, '#321414'] // brown
      ]
    } , {
      pane: 1,
      lineWidth: 0,
      tickWidth: 0,
      minorTickInterval: null,
      tickAmount: 2,
      title: {
        text: 'Temperature',
        y: 63,
        style: {
          fontSize: "1.3em"
        }
      },
      labels: {
        enabled: true,
        distance: '90%',
        y: 16
      },
      min: 30,
      max: 90,
      stops: [
        [0.1, '#0000ff'], // blue
        [0.5, '#ffff00'], // yellow
        [0.9, '#ff0000'] // red
      ]
    } , {
      pane: 2,
      lineWidth: 0,
      tickWidth: 0,
      minorTickInterval: null,
      tickAmount: 2,
      title: {
        text: 'Battery',
        y: 63,
        style: {
          fontSize: "1.3em"
        }
      },
      labels: {
        enabled: true,
        distance: '90%',
        y: 16
      },
      min: 0,
      max: 100,
      stops: [
        [0.1, '#ff0000'], // red
        [0.5, '#0000ff'], // blue
        [0.9, '#008000'] // green
      ]
    }],

    plotOptions: {
      solidgauge: {
        dataLabels: {
          style: {
            fontSize: '0.8em'
          },
          y: -35,
          borderWidth: 0,
          useHTML: true
        }
      }
    }
  };

  const chart = Highcharts.chart('chart', Highcharts.merge(gaugeOptions, {
      series: [{
        name: 'Tilt',
        data: [0],
        dataLabels: {
            format:
              '{y:.1f}&deg;'
        },
        tooltip: {
          valueSuffix: ' &deg;'
        },
        yAxis: 0
      }, {
        name: 'Temperature',
        data: [30],
        dataLabels: {
            format:
              '{y:.1f}&deg;F'
        },
        tooltip: {
          valueSuffix: ' &deg;F'
        },
        yAxis: 1
      }, {
        name: 'Battery',
        data: [0],
        dataLabels: {
          format: '{y}%'
        },
        tooltip: {
          valueSuffix: ' %'
        },
        yAxis: 2
      }]
  }));

  var pause = false;

  $(document).ready(function(){
    $('.chart-container').hide();
    var failureOptions = "";
    for (var i = 1; i<11; i++ ) {
        failureOptions += "<option value=" + i + ">" + i + "</option>";
    }
    $("#failures").html( failureOptions );
    $("#failures").val("5").change();
    setInterval(function(){
      $.getJSON( "/state", function( data ) {
        if (!pause) {
          $('#loading').hide();
          $('.chart-container').show();
          if (chart && "angle" in data) {
            chart.series[0].points[0].update(parseFloat(data["angle"]));
          }
          if (chart && "temperature" in data) {
            chart.series[1].points[0].update(parseFloat(data["temperature"]));
          }
          if (chart && "battery" in data) {
            chart.series[2].points[0].update(parseInt(data["battery"]));
          }
        }
      });
      chart.reflow();
    }, 1000);

    $("#configuration input").each(function () {
      $(this).rules("add", {
        required: true
      });
    });
    $('.loggingForm').hide();

    $("#btnReset").click(function(){
      if (confirm('Are you sure you want to reset all the settings?')) {
        $.post( "/reset", function( data ) {
          $('body').html("<pre style=\"word-wrap: break-word; white-space: pre-wrap;\"></pre>");
          $('body > pre').html(data);
        });
      }
    });

    $("#btnStandby").click(function(){
      if (confirm('Are you sure you want to put the Hydrometer into standby mode?')) {
        $.post( "/standby", function( data ) {
          $('body').html("<pre style=\"word-wrap: break-word; white-space: pre-wrap;\"></pre>");
          $('body > pre').html(data);
        });
      }
    });

    $("#btnTare").click(function(){
      if (confirm('Please set The-Floater on its head so that it is perfectly perpendicular.')) {
        $.post( "/tare", function(data) {
          pause = true;
          $('#loading').show();
          $('.chart-container').hide();
          setTimeout(() => pause = false, 3000);
        });
      }
    });

    $.getJSON( "/logging_info", function( data ) {
      if (data.hasOwnProperty("time")) {
        $("#time").val(data["time"]);
      }
      if (data.hasOwnProperty("type")) {
        $("#logType").val(data["type"]).change();
        if (data.hasOwnProperty("iftttKey")) {
          $("#iftttKey").val(data["iftttKey"]);
          $("#iftttEvent").val(data["iftttEvent"]);
        }
        if (data.hasOwnProperty("brewersFriendKey"))
          $("#brewersFriendKey").val(data["brewersFriendKey"]);
        if (data.hasOwnProperty("bfSquaredCoeff"))
          $("#bfSquaredCoeff").val(data["bfSquaredCoeff"].toFixed(6));
        if (data.hasOwnProperty("bfFirstDegreeCoeff"))
          $("#bfFirstDegreeCoeff").val(data["bfFirstDegreeCoeff"].toFixed(6));
        if (data.hasOwnProperty("bfZeroDegreeCoeff"))
          $("#bfZeroDegreeCoeff").val(data["bfZeroDegreeCoeff"].toFixed(6));

        if (data.hasOwnProperty("sheetsDeploymentId"))
          $("#sheetsDeploymentId").val(data["sheetsDeploymentId"]);
        if (data.hasOwnProperty("sheetsSheetId"))
          $("#sheetsSheetId").val(data["sheetsSheetId"]);
        if (data.hasOwnProperty("sheetsSheetName"))
          $("#sheetsSheetName").val(data["sheetsSheetName"]);
      }
    });
  });

  $('#logType').on('change', function (e) {
    var valueSelected = this.value;
    $('.loggingForm').hide();
    $('.loggingForm *').attr('disabled', true);
    switch (valueSelected) {
      case "ifttt":
        $('#logging-ifttt').show();
        $('#logging-ifttt *').attr('disabled', false);
        break;
      case "brewersFriend":
        $('#logging-brewersFriend').show();
        $('#logging-brewersFriend *').attr('disabled', false);
        break;
      case "googleSheets":
        $('#logging-googleSheets').show();
        $('#logging-googleSheets *').attr('disabled', false);
        break;
      default:
        console.log("oops, bad value");
    }
  });
  $("#configuration").validate({
    errorClass: "my-error-class",
    errorPlacement: function(error, element) {
      $('#errorMessage').html(error);
    },
    rules: {
      time: {
        required: true,
        min: {
          param: 15,
          depends: function(element) {
            return $("#logType").val() === "brewersFriend";
          }
        }
      }
    }
  });

});

