document.addEventListener('DOMContentLoaded', function () {
    const gaugeOptions = {
        chart: {
            type: 'solidgauge'
        },

        title: null,

        pane: {
            center: ['50%', '85%'],
            size: '120%',
            startAngle: -90,
            endAngle: 90,
            background: {
                backgroundColor:
                    Highcharts.defaultOptions.legend.backgroundColor || '#EEE',
                innerRadius: '60%',
                outerRadius: '100%',
                shape: 'arc'
            }
        },

        credits: {
            enabled: false
        },

        // the value axis
        yAxis: {
            lineWidth: 0,
            tickWidth: 0,
            minorTickInterval: null,
            tickAmount: 2,
            title: {
                y: -45,
                style: {
                    fontSize: "1.2em"
                }
            },
            labels: {
                y: 16
            }
        },

        plotOptions: {
            solidgauge: {
                dataLabels: {
                    style: {
                        fontSize: '0.8em'
                    },
                    y: -25,
                    borderWidth: 0,
                    useHTML: true
                }
            }
        }
    };

    // The tilt gauge
    const chartTilt = Highcharts.chart('chart-tilt', Highcharts.merge(gaugeOptions, {
        yAxis: {
            min: 0,
            max: 90,
            tickPositions: [0, 90],
            title: {
                text: 'Tilt'
            },
            stops: [
            [0.1, '#ffff00'], // yellow
            [0.5, '#ff8c00'], // orange
            [0.9, '#321414'] // brown
            ],
        },

        series: [{
            name: 'Tilt',
            data: [0],
            dataLabels: {
                format:
                    '{y:.1f}&deg;'
            },
            tooltip: {
                valueSuffix: ' &deg;'
            }
        }]
    }));

    // The Temperature gauge
    const chartTemperature = Highcharts.chart('chart-temperature', Highcharts.merge(gaugeOptions, {
        yAxis: {
            min: 30,
            max: 90,
            title: {
                text: 'Temperature'
            },
            stops: [
            [0.1, '#0000ff'], // blue
            [0.5, '#ffff00'], // yellow
            [0.9, '#ff0000'] // red
            ],
        },

        series: [{
            name: 'Temperature',
            data: [30],
            dataLabels: {
                format:
                    '{y:.1f}&deg;F'
            },
            tooltip: {
                valueSuffix: ' &deg;F'
            }
        }]
    }));

    // The Battery gauge
    const chartBattery = Highcharts.chart('chart-battery', Highcharts.merge(gaugeOptions, {
        yAxis: {
            min: 0,
            max: 100,
            title: {
                text: 'Battery'
            },
            stops: [
            [0.1, '#ff0000'], // red
            [0.5, '#0000ff'], // blue
            [0.9, '#008000'] // green
            ],

        },

        series: [{
            name: 'Battery',
            data: [100],
            dataLabels: {
                format: '{y}%'
            },
            tooltip: {
                valueSuffix: ' %'
            }
        }]
    }));


    $(document).ready(function(){
        $('.chart-container').hide();
        var failureOptions = "";
        for (var i = 1; i<11; i++ ) {
            failureOptions += "<option value=" + i + ">" + i + "</option>";
        }
        $("#failures").html( failureOptions );
        $("#failures").val("5").change();
        setInterval(function(){
          $('#loading').hide();
          $('.chart-container').show();
    
          $.getJSON( "/state", function( data ) {
            if (chartTilt && "angle" in data) {
              chartTilt.series[0].points[0].update(parseFloat(data["angle"]));
            }
            if (chartTemperature && "temperature" in data) {
              chartTemperature.series[0].points[0].update(parseFloat(data["temperature"]));
            }
            if (chartBattery && "battery" in data) {
              chartBattery.series[0].points[0].update(parseInt(data["battery"]));
            }
          });
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
            if (data.hasOwnProperty("brewersFriendKey")) {
              $("#brewersFriendKey").val(data["brewersFriendKey"]);
            }
          }
        });
    });
});

