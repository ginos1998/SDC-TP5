package sdc.tp5.gui;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.stage.Stage;
import javafx.util.Duration;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class HelloApplication extends Application {

  private static final String MEMINFO_PATH = "/proc/meminfo";
  private static final int UPDATE_INTERVAL_MS = 500;

  private XYChart.Series<Number, Number> series;
  private long startTime;

  @Override
  public void start(Stage primaryStage) {
    NumberAxis xAxis = new NumberAxis();
    NumberAxis yAxis = new NumberAxis();

    LineChart<Number, Number> lineChart = new LineChart<>(xAxis, yAxis);
    lineChart.setTitle("Memoria RAM Disponible");
    lineChart.setAnimated(false);

    series = new XYChart.Series<>();
    lineChart.getData().add(series);

    Scene scene = new Scene(lineChart, 800, 400);
    primaryStage.setScene(scene);
    primaryStage.show();

    startTime = System.currentTimeMillis();

    Timeline timeline = new Timeline(new KeyFrame(Duration.millis(UPDATE_INTERVAL_MS), event -> {
      updateChartData();
    }));
    timeline.setCycleCount(Timeline.INDEFINITE);
    timeline.play();
  }

  private void updateChartData() {
    try (BufferedReader reader = new BufferedReader(new FileReader(MEMINFO_PATH))) {
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