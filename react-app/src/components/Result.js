import React from "react";
import { connect } from "react-redux";
import { setActiveComponent, restart } from "../actions";

function Result({ result, attributes, mask, numQuestions, restart }) {
  let ths = [];
  attributes.forEach(([attr, config]) => {
    if (mask[attr]) {
      const th = <th key={attr}>{attr}</th>;
      ths.push(th);
    }
  });
  const tds = result.map((x, i) => <td key={i}>{x}</td>);
  return (
    <div className="row justify-content-center">
      <div className="col">
        <h4>No. of questions asked: {numQuestions}</h4>
        <h4>Your favorite car is</h4>
        <table className="table table-hover text-center">
          <thead>
            <tr>{ths}</tr>
          </thead>
          <tbody>
            <tr>{tds}</tr>
          </tbody>
        </table>
        <button type="button" className="btn btn-primary" onClick={restart}>
          Return to Welcome
        </button>
      </div>
    </div>
  );
}

const mapStateToProps = ({ result, attributes, mask, numQuestions }) => ({
  result,
  attributes,
  mask,
  numQuestions
});

const mapDispatchToProps = dispatch => ({
  restart: () => {
    dispatch(setActiveComponent("Welcome"));
    dispatch(restart());
  }
});

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(Result);
