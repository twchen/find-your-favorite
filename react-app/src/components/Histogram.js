import React from "react";
import { connect } from "react-redux";
import { XYPlot, XAxis, YAxis, Hint, VerticalBarSeries, ChartLabel } from 'react-vis';

class Histogram extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      hintValue: null
    }
    this.data = props.numLeftPoints.map((num, i) => ({ x: i, y: num }));
  }

  _onMouseLeave = () => {
    this.setState({
      hintValue: null
    });
  }

  _onNearestX = (value, { index }) => {
    this.setState({
      hintValue: this.data[index]
    });
  }

  render() {
    return (
      <XYPlot onMouseLeave={this._onMouseLeave} height={400} width={400}>
        <XAxis
          tickValues={this.props.numLeftPoints.map((num, i) => i)}
          tickFormat={value => value.toString()}
        />
        <YAxis />
        <ChartLabel
          text="Question No."
          includeMargin={false}
          xPercent={0.82}
          yPercent={1.06}
        />
        <ChartLabel
          text="No. of Cars Left"
          includeMargin={false}
          xPercent={-0.01}
          yPercent={0.05}
          style={{
            transform: 'rotate(-90)',
            textAnchor: 'end'
          }}
        />
        <VerticalBarSeries color='rgb(0, 123, 255)' onNearestX={this._onNearestX} data={this.data} />
        {
          this.state.hintValue &&
          <Hint value={this.state.hintValue}>
            <p style={{ color: 'rgb(90, 90, 90)' }}>Q{this.state.hintValue.x}: {this.state.hintValue.y} Cars Left</p>
          </Hint>
        }
      </XYPlot>
    );
  }

}

const mapStateToProps = ({ numLeftPoints }) => ({ numLeftPoints });

export default connect(mapStateToProps)(Histogram);