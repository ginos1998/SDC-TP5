module sdc.tp5.gui {
  requires javafx.controls;
  requires javafx.fxml;

  requires org.controlsfx.controls;
  requires org.kordamp.bootstrapfx.core;
  requires javafx.swing;

  opens sdc.tp5.gui to javafx.fxml;
  exports sdc.tp5.gui;
}