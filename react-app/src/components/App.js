import React from "react";
import Welcome from "./Welcome";
import Interaction from "./Interaction";
import Result from "./Result";
import Stats from "./Stats";
import { connect } from "react-redux";
import Visuals from "./Visuals";

function App({ activeComponent }) {
  if (activeComponent === "Welcome") {
    return <Welcome />;
  } else if (activeComponent === "Interaction") {
    return (
      <div>
        <Interaction />
        <Visuals />
        <Stats />
      </div>
    );
  } else if (activeComponent === "Result") {
    return <Result />;
  } else {
    return <div />;
  }
}

const mapStateToProps = ({ activeComponent }) => ({
  activeComponent
});

export default connect(mapStateToProps)(App);
