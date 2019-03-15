import React from "react";
import { connect } from "react-redux";
import Histogram from './Histogram';
import ConvextHull from './ConvexHull';

class Visuals extends React.Component {
  constructor(props) {
    super(props);
    this.attributes = [];
    this.props.attributes.forEach(([attr, config]) => {
      if (this.props.mask[attr]) {
        this.attributes.push(attr);
      }
    });
  }

  render() {
    if (this.attributes.length === 4) {
      return (
        <div className="row" style={{minWidth: '1000px'}}>
          <div className="col-6">
            <h4>No. of Cars Left vs. No. of Questions Asked</h4>
            <div className="d-flex justify-content-center">
              <Histogram />
            </div>
          </div>
          <div className="col-6">
            <h4>Convex Hull of Preference Plane</h4>
            <ConvextHull />
          </div>
        </div>
      );
    } else {
      return <div />;
    }

  }

}

const mapStateToPropsStats = ({
  attributes,
  mask
}) => ({
  attributes,
  mask
});

export default connect(mapStateToPropsStats)(Visuals);