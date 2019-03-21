import React from "react";
import Histogram from "./Histogram";
import PreferencePlane from "./PreferencePlane";

function Visuals() {
  return (
    <div>
      <h2>Visuals</h2>
      <div className="row justify-content-center">
        <div className="col-6" style={{ minWidth: "500px" }}>
          <Histogram />
        </div>
        <div className="col-6" style={{ minWidth: "500px" }}>
          <PreferencePlane />
        </div>
      </div>
    </div>
  );
}

export default Visuals;
