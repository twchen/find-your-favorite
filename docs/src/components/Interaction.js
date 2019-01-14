import { setActiveComponent, setResult } from "../actions";
import { connect } from "react-redux";

class Interaction extends React.Component {
  constructor(props) {
    super(props);
    const smallerBetter = new Module.VectorInt();
    this.attributes = [];
    this.props.attributes.forEach(([attr, config]) => {
      if(this.props.mask[attr]){
        smallerBetter.push_back(config.smallerBetter);
        this.attributes.push(attr);
      }
    })
    this.runner = new Module.AlgorithmRunner(this.props.candidates, smallerBetter);
    this.state = {
      pair: this.runner.nextPair()
    }
  }

  choose = (idx) => {
    const option = idx + 1;
    this.runner.choose(option);
    if(this.runner.isFinished()){
      const result = this.runner.getResult();
      this.props.showResult(result);
    }else{
      const nextPair = this.runner.nextPair();
      this.setState({
        pair: nextPair,
        numAsked: this.state.numAsked + 1
      })
    }
  }

  render() {
    let trs = [];
    for(let i = 0; i < this.state.pair.size(); ++i){
      const point = this.state.pair.get(i);
      let tds = [<td key="Option No.">{i + 1}</td>];
      for(let j = 0; j < point.dim(); ++j){
        tds.push(
          <td key={this.attributes[j]}>
            {point.get(j)}
          </td>
        );
      }
      tds.push(
        <td key="Choose button">
          <button
            type="button"
            className="btn btn-outline-success btn-sm"
            onClick={() => this.choose(i)}
          >
            Choose
          </button>
        </td>
      )
      const tr = <tr key={i}>{tds}</tr>;
      trs.push(tr);
    }
    let ths = [<th key="Option No.">Option</th>];
    this.attributes.forEach(attr => {
      ths.push(<th key={attr}>{attr}</th>);
    });
    ths.push(<th key="chooseButton" />);
    return (
      <div className="container">
        <div className="row justify-content-center">
          <div className="col-md-8">
            <h4>Please choose the car you favor more: </h4>
            <table className="table table-hover text-center">
              <thead>
                <tr>{ths}</tr>
              </thead>
              <tbody>{trs}</tbody>
            </table>
          </div>
        </div>
      </div>
    );
  }
}

const mapStateToProps = ({candidates, mask, attributes}) => ({
  candidates,
  mask,
  attributes
});

const mapDispatchToProps = dispatch => ({
  showResult: result => {
    dispatch(setResult(result));
    dispatch(setActiveComponent('Result'))
  }
})

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(Interaction);