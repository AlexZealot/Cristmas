//иконка - ТВ
const char wpi_html [] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width" initial-scale="1" charset="utf-8">
    <title>Подсветка телевизора</title>
    <link rel="icon" href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAwnpUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHjabVBBEsMgCLz7ij5BWaL4HNOkM/1Bn18UkolNd8YVWGZFwv55v8KjgxIHXorkmnNUcOVKTQOJhjY4RR48QC5pPtXDKZCWoDcslez9Rz2dBnY1jZaLkTxdWGehsvvLj5G/jD5Rjzc3qm4EMiG5QbNvxVylXL+w7nGG2AmdWOaxb3nR7W2LvgOiHQlRGWAbAP0goHVBmSC9UbMGRhmc3EwX8m9PB8IX2L1ZDRkZVnkAAAGFaUNDUElDQyBwcm9maWxlAAB4nH2RPUjDQBzFX1OlUqoOdhCpkKG6aBcVcSxVLIKF0lZo1cHk0i9o0pC0uDgKrgUHPxarDi7Oujq4CoLgB4iri5Oii5T4v6TQIsaD4368u/e4ewcIzQpTzZ4ooGo1IxWPidncquh7hR8hDGACoxIz9UR6MQPX8XUPD1/vIjzL/dyfo1/JmwzwiMRRphs14g3i2c2aznmfOMhKkkJ8Tjxp0AWJH7kuO/zGuWizwDODRiY1TxwkFotdLHcxKxkq8QxxWFE1yheyDiuctzirlTpr35O/MJDXVtJcpxlCHEtIIAkRMuooo4IaIrRqpJhI0X7MxT9i+5PkkslVBiPHAqpQIdl+8D/43a1ZmJ5ykgIxoPfFsj7GAN8u0GpY1vexZbVOAO8zcKV1/NUmMPdJeqOjhY+AwW3g4rqjyXvA5Q4w/KRLhmRLXppCoQC8n9E35YChW8C/5vTW3sfpA5ChrpZvgINDYLxI2esu7+7r7u3fM+3+fgDxR3LZCN9eXQAADXZpVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADw/eHBhY2tldCBiZWdpbj0i77u/IiBpZD0iVzVNME1wQ2VoaUh6cmVTek5UY3prYzlkIj8+Cjx4OnhtcG1ldGEgeG1sbnM6eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IlhNUCBDb3JlIDQuNC4wLUV4aXYyIj4KIDxyZGY6UkRGIHhtbG5zOnJkZj0iaHR0cDovL3d3dy53My5vcmcvMTk5OS8wMi8yMi1yZGYtc3ludGF4LW5zIyI+CiAgPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIKICAgIHhtbG5zOnhtcE1NPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvbW0vIgogICAgeG1sbnM6c3RFdnQ9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlwZS9SZXNvdXJjZUV2ZW50IyIKICAgIHhtbG5zOmRjPSJodHRwOi8vcHVybC5vcmcvZGMvZWxlbWVudHMvMS4xLyIKICAgIHhtbG5zOkdJTVA9Imh0dHA6Ly93d3cuZ2ltcC5vcmcveG1wLyIKICAgIHhtbG5zOnRpZmY9Imh0dHA6Ly9ucy5hZG9iZS5jb20vdGlmZi8xLjAvIgogICAgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvIgogICB4bXBNTTpEb2N1bWVudElEPSJnaW1wOmRvY2lkOmdpbXA6Yjg5NDNmM2MtNTg1Mi00ZGViLTljNWEtMzliYzNkM2U5YWQ5IgogICB4bXBNTTpJbnN0YW5jZUlEPSJ4bXAuaWlkOmY0ZDk5Yzg4LTQ1YTYtNGJlNS1hYjc4LWRkZjk5NTVhNTkzOSIKICAgeG1wTU06T3JpZ2luYWxEb2N1bWVudElEPSJ4bXAuZGlkOjBhNjc4ZGJkLWYzZjQtNDlhOS1iM2UyLWVhMzk4ZDVhNWYyMiIKICAgZGM6Rm9ybWF0PSJpbWFnZS9wbmciCiAgIEdJTVA6QVBJPSIyLjAiCiAgIEdJTVA6UGxhdGZvcm09IldpbmRvd3MiCiAgIEdJTVA6VGltZVN0YW1wPSIxNjk4NTA0NDYyNzE2MDc2IgogICBHSU1QOlZlcnNpb249IjIuMTAuMzQiCiAgIHRpZmY6T3JpZW50YXRpb249IjEiCiAgIHhtcDpDcmVhdG9yVG9vbD0iR0lNUCAyLjEwIgogICB4bXA6TWV0YWRhdGFEYXRlPSIyMDIzOjEwOjI4VDIxOjQ3OjQxKzA3OjAwIgogICB4bXA6TW9kaWZ5RGF0ZT0iMjAyMzoxMDoyOFQyMTo0Nzo0MSswNzowMCI+CiAgIDx4bXBNTTpIaXN0b3J5PgogICAgPHJkZjpTZXE+CiAgICAgPHJkZjpsaQogICAgICBzdEV2dDphY3Rpb249InNhdmVkIgogICAgICBzdEV2dDpjaGFuZ2VkPSIvIgogICAgICBzdEV2dDppbnN0YW5jZUlEPSJ4bXAuaWlkOmZmNjY4MWUxLWY5MDEtNDkzMC05M2EwLTY3OWU1OWVkOTM3YyIKICAgICAgc3RFdnQ6c29mdHdhcmVBZ2VudD0iR2ltcCAyLjEwIChXaW5kb3dzKSIKICAgICAgc3RFdnQ6d2hlbj0iMjAyMy0xMC0yOFQyMTo0Nzo0MiIvPgogICAgPC9yZGY6U2VxPgogICA8L3htcE1NOkhpc3Rvcnk+CiAgPC9yZGY6RGVzY3JpcHRpb24+CiA8L3JkZjpSREY+CjwveDp4bXBtZXRhPgogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgCjw/eHBhY2tldCBlbmQ9InciPz5o7aBzAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH5wocDi8qVNOtrQAACDBJREFUWMO1l02MHEcVx3+vurpnenZnd+3NOrENxkZJFJyQDwVQAiJ8CI4QsDggQCDxLS45cIAcOCGQIsGBC1xAIkLkhlC4BCEIHwkJBAsRkQQ7Jo6TXa+93k/vzmxPd1W9x2FmZycRiOSQHj31m9fVVf9+71//qoLXeNVPfX6l/usXb/u/7R7/xPvqv31p8bX2615rQ6vka6vX3n/v0tLy3P9qs7S0fPDioW+/3yr5Km/Ede7cvzsXLrz8w6Wl5be/+tni4sXbzp4994Mnn/xL+Xr6lNcL4plnnvNTU1M/6Mbzz8z2H3or4tNm59OLq838Tevra/fdc8979Q0FAND89ZMnrTz+rLS6gKChNheuXF/c8aPzr7evMYDw4k8Oovoei+E4pjdbiiVwAykWpikTOGRwELM2qmAG2ERPDpyAuAGwAXZFXJZwWQN2DskqTJ+VvLggRf7n/MRXNsYA4tLPvx6vLD4QVy9luruDVX1QRbKcjY2rRDziPFU1oN+vqENCDcQ5xGVYSiCOPPO02wVTUy3yPENDRaft6c7NYCkg7RJXlrhOmeLGpW90P/Kz70s4/+O70taVJzef+B2nz17lpV6LNTfN489vsbZTY2ao6thSSq/4v2dmNrY8z/He02636Xa7FEXBTUe63DCfc/M1Ge98U42f8WxfXr/bx7XFe8PaJb73aJ8/LQqqA44eLbj5jsD8Qc/Becff0+cQJ/iiRV4+jfPrqBpvPXOS6TOzBIuU9Qr51dNc6G2zbjkPLp0nrq6iqhw7doznVDl7KePX3vOh66f48j1GtdW/11tdHX7pyg6PLSmnTp3i1KlTzM8b7fw+XCY4J/yi9xkQQYAqZqi9BMDtnbs4XL8ZgPbmWaZ3nkXmOqxdcwebn/gcOxsv88TD3+Ly5WeYmZnhhmMzvOuWw/zqjy/xsTtnsSw77BC9cWt7wPT0NJ/61Kc5fvw43e40rXZGnjuyTMA5EMFEMAM1G5mimlBNmCYsRrQJpKpma2eAFtdy03u+gojQNIHPf/xWPnRkwGdvjqxtR8RnN3pSKta3G06ePMnMzMyQ3IDJ/nSOahN+ImkEIGkc+0MQw3dME5qG8fb0IdrtNikl5Ok/cfrCFTbrkmuvPYg4V3g0tqtBZGpqGufcaGoZ2D6AoBNgkpI0jfxIGg2kKe6/o4qN4phQliUi8NBTFUcoOB3nmN+oODpP25vZdaZGjGHEaEPEgPRfMxBSQvcykCIphjEAGwEzTaQ0jIt4Op0OWZZx9M63cffJa1h+5AzmWxhc5zHtxmiIOKpqgIiQuYi4ZiguQJMSMtKsSQAxBeJooBgDKQXMIAx2qba3iDFgoRplwHHvOxZYe/affGB+F/UzYNb1qBZ1BOcdZkYIgUiNydb4q1+4vDH2B70VLA5nwcLF64iLg6HfWyS/uglAlTbZWrs85EDhaLfbiAgvPPoHBlfWeTEd4i1zgFnhUSUmQXIZiY7hMMztl6AO+xxomoiO0t6EmjoMAYTYoKMSaIqkUVxdRlmWpJR48PkZjvppLvhZjiQBM7yZgmSIyFjVEB0zGqCO+2CakMYA6jigiYNRCWo06QhAQEdx9Z7BYECv18PNz3PBFxRFgeLBDIcqLssRkQnZNUzj2OowYU2kaQJNEwihpgkDmjAYAhhpQkqBGAbEMEBjzWOPPUae5xTFcPA8z3Euw0zxqJL7fJyBoZ4r5v5HBpqIxWZUmv0SxIkS2EQJrBjOoD0AeZ4P/TwDNTym1m4VIknGX48YZpMc2PdDSFjY58C4BKme4EAgjeJ70mBm1PVwcQNwGjHUvKlWZavoWB9SSng/ysKEDkxmIIaEhTgm3j4HmiF/ANWI7mUgMh50d3eXGCMbGxuk412spZU3TStmekJVCSGS5zmgGBNKGP97BkKsCbEeidKrZsEo3gyGbcuyRFXp9/tUVYXoDKq24km6GXqXT5w9OyCEgGobeVUJmjQhxVEnMhAIIz6kGCbWgkgaxYVsqAftNtvb2/T7fUSEg1MZNtBNj6a0MJ2xsnKZnZ1tpqenMYZE3M/AqwDEfSUMaQRAw0QJEpoaBCHWO5gZS0tLr9gLtj2kGJOzpnmhW2b0ej0eeeQRlpeX6fX6hDCajmaEpGOLUUkxkWIaS3FMYbgW7P0EWoUwN5OxsfovRISiKPYPI85xoONJdXzBa7+/emxGODbrePjhX/Kb3/yao0fmOHLIc+CAMjsT2Fp/AvEeKdrk9RqiChi97YbM9wiacL7HTpGzmiJrbNNs/5Plc2dIseLEiROUZUlZDo8Md15/HXNpjcWqWZWVB27/mLRbv1x+uc9vn088Fzo8/mIPndj7pdfgT+4N9+4LCwvMzc2xsLAAwK0nb+SOQ573dnvUK0+xvlF9XAD+df9Nj2L2AR8jmSnFgTez1Wuw1jQ7dZuB1ShGkBlSMYflLUKzO1wVZUi13Jf4vAWDPj5u0cqNTvcwWbXFVOloqcLOeaq1JfpVoi/Z79//04sf9ADnL/Y+3CnkozqI764jX+hsXDiQCfjMyAS6o/27ACKGIMjeicIMs9E2Rm14H23bkkKjxk4SksKgCjsZ6Ucp6JPBya8APMA/Tq/lYGdA2mWZ3T43W54wVbe5NXixbnTZ567bmfaHnGMWo2NqLTNyzIZbKCEK0ohjF5Geqm1V/XQpBN12To4uzJcnXeZkt1+/fHUn/B04A9L67i2zAwH4zttyaUfLYjvLLFpmhjeRDMEb5CBewGPiEfM2PFU7Adn7eAQVkwgWBWLCooNoJtGZRUOj9y5ltWpwMdEu9ZtP79p/AIpFxKVdHG5fAAAAAElFTkSuQmCC" type="image/x-icon">
    <style>
      html {font-family: Arial; display: inline-block; text-align: center; color: #FFFFFF}
      body {max-width: 400px; margin:0px auto; padding-bottom: 25px; background-color: #000000}
      select {height: 30px; border: solid; border-width: 1px; border-radius: 3px; width: 300px}
	    .modes {border: solid; border-color: #ffffff; border-width: 1px; border-radius: 10px; padding-left: 10px; padding-bottom: 10px; margin: 10px}
      .THP {border: solid; border-color: #ffffff; border-width: 1px; border-radius: 10px; padding-left: 10px; padding-bottom: 10px; margin: 10px}
    </style>
    <script>  
      setInterval(loadTemp, 5000);
      function loadTemp() {
        nocache = "&nocache="+ Math.random() * 1000000;
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("THP").innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "/refT", true);
        xhttp.send();
      }
	  
	    function setMode(element){
		    var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function (){};
		    xhttp.open("POST", "/setMode?mode="+element.value, true);
		    xhttp.send();
	    }

      setInterval(loadMode, 500);
      function loadMode(){
        nocache = "&nocache="+ Math.random() * 1000000;
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("mode").innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "/refMode", true);
        xhttp.send();        
      }
    </script>
  </head>
  <body>
    <div align=center class="modes">
	  <h1>Режим гирлянды</h1>
    <h2>Подсветка под ТВ</h2>
      <select id="mode" onchange="setMode(this)">
)rawliteral";	  
		
const char wpi_html_mid [] PROGMEM = R"rawliteral(		
      </select>
    </div>
			
	<div align=center class="THP" id="THP">
)rawliteral";

const char wpi_html_end [] PROGMEM = R"rawliteral(
	</div>
  </body>
</html>
)rawliteral";

const char THP_html [] = R"rawliteral(
	  <p>Температура: %.1f&deg;C
    <p>Влажность: %.2f %%
    <p>Давление: %.2f мм. рт. ст. %s
    <p><br>
    <p>Потребляемый ток: %.3f А
    <p>Яркость: %i
    <p><br>
    <p><br>
    <p><br>
    <p>Время работы: %02i:%02i:%02i
)rawliteral";

const char no_THP_html [] = R"rawliteral(
    <p>Потребляемый ток: %.3f А
    <p>Яркость: %i
    <p><br>
    <p><br>
    <p><br>
    <p>Время работы: %02i:%02i:%02i
)rawliteral";