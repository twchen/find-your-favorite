import React from "react";
import { connect } from "react-redux";
import { XYPlot, XAxis, YAxis, Hint, VerticalBarSeries, ChartLabel } from 'react-vis';

class Histogram extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      hintValue: null
    }
  }

  _onMouseLeave = () => {
    this.setState({
      hintValue: null
    });
  }

  _onNearestX = (value, { index }) => {
    this.setState({
      hintValue: this.props.data[index]
    });
  }

  render() {
    return (
      <XYPlot onMouseLeave={this._onMouseLeave} height={400} width={400}>
        <XAxis
          tickValues={this.props.data.map((val, idx) => idx)}
          tickFormat={value => value.toString()}
          style={{
            ticks: { fill: 'black' }
          }}
        />
        <YAxis
          style={{
            ticks: { fill: 'black' }
          }}
        />
        <ChartLabel
          text="Question No."
          includeMargin={false}
          xPercent={0.82}
          yPercent={1.06}
          style={{
            style: { fill: 'black' }
          }}
        />
        <ChartLabel
          text="No. of Cars Left"
          includeMargin={false}
          xPercent={-0.01}
          yPercent={0.05}
          style={{
            transform: 'rotate(-90)',
            textAnchor: 'end',
            style: { fill: 'black' }
          }}
        />
        <VerticalBarSeries color='rgb(0, 123, 255)' onNearestX={this._onNearestX} data={this.props.data} animation />
        {
          this.state.hintValue &&
          <Hint value={this.state.hintValue}>
            <p style={{ color: 'black' }}>Q{this.state.hintValue.x}: {this.state.hintValue.y} Cars Left</p>
          </Hint>
        }
      </XYPlot>
    );
  }

}

const mapStateToProps = ({ numLeftPoints }) => ({
  data: numLeftPoints.map((num, i) => ({ x: i, y: num }))
});

export default connect(mapStateToProps)(Histogram);