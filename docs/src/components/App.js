import React from 'react';
import classNames from 'classnames';
import Welcome from './Welcome'
import Interaction from './Interaction'
import Result from './Result'
import { connect } from "react-redux";

function App({activeComponent}){
  return (
    <div>
      <Welcome className={classNames({'hidden': activeComponent !== 'Welcome'})} />
      <Interaction className={classNames({'hidden': activeComponent !== 'Interaction'})} />
      <Result className={classNames({'hidden': activeComponent !== 'Result'})} />
    </div>
  )
}

const mapStateToProps = ({activeComponent}) => ({
  activeComponent
});

export default connect(mapStateToProps)(App);