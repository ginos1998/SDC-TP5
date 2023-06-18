package sdc.tp5.gui;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Button;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.stage.Stage;
import javafx.util.Duration;
import sdc.tp5.gui.utils.Constants;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class HelloApplication extends Application {

  private XYChart.Series<Number, Number> series;
  private long startTime;

  @Override
  public void start(Stage primaryStage) {
    primaryStage.setTitle(Constants.WINDOW_TITTLE);

    // Crear el gráfico de línea
    NumberAxis xAxis = new NumberAxis();
    NumberAxis yAxis = new NumberAxis();

    LineChart<Number, Number> lineChart = new LineChart<>(xAxis, yAxis);
    lineChart.setTitle(Constants.GRAPH_TITLE);
    lineChart.setAnimated(false);

    series = new XYChart.Series<>();
    lineChart.getData().add(series);

    // Crear los botones A y B
    Button buttonA = new Button(Constants.BUTTON_A);
    Button buttonB = new Button(Constants.BUTTON_B);

    // Crear la subventana superior con los botones
    HBox buttonBox = new HBox(10, buttonA, buttonB);
    buttonBox.setPadding(new Insets(10));
    buttonBox.setPrefHeight(400);

    // Crear la subventana inferior con el gráfico
    BorderPane chartPane = new BorderPane(lineChart);
    chartPane.setPadding(new Insets(10));
    chartPane.setPrefHeight(400);

    // Crear el contenedor principal
    BorderPane root = new BorderPane();
    root.setTop(buttonBox);
    root.setBottom(chartPane);

    // Crear la escena
    Scene scene = new Scene(root, Constants.WINDOW_HEIGTH, Constants.WINDOW_WIDTH);

    // Configurar el escenario principal
    primaryStage.setScene(scene);
    primaryStage.show();

    startTime = System.currentTimeMillis();

    // Iniciar la actualización del gráfico en tiempo real
    Timeline timeline = new Timeline(new KeyFrame(Duration.millis(Constants.UPDATE_INTERVAL_MS), event -> {
      updateChartData();
    }));
    timeline.setCycleCount(Timeline.INDEFINITE);
    timeline.play();
  }

  private void updateChartData() {
    try (BufferedReader reader = new BufferedReader(new FileReader(Constants.FILE_PATH))) {
      String line;
      while ((line = reader.readLine()) != null) {
        if (line.startsWith("MemAvailable:")) {
          long availableMemoryKB = extractAvailableMemory(line);
          Platform.runLater(() -> {
            long currentTime = System.currentTimeMillis();
            double elapsedTimeInSeconds = (currentTime - startTime) / 1000.0;
            series.getData().add(new XYChart.Data<>(elapsedTimeInSeconds, availableMemoryKB));
          });
          break;
        }
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  private long extractAvailableMemory(String line) {
    Pattern pattern = Pattern.compile("(\\d+)");
    Matcher matcher = pattern.matcher(line);
    if (matcher.find()) {
      return Long.parseLong(matcher.group(1));
    }
    return 0;
  }

  public static void main(String[] args) {
    launch(args);
  }
}