import { connect } from "react-redux";

function Result({result, attributes, mask}){
  let ths = [];
  attributes.forEach(([attr, config]) => {
    if(mask[attr]){
      const th = <th key={attr}>{attr}</th>;
      ths.push(th);
    }
  });
  let tds = [];
  for(let i = 0; i < result.dim(); ++i){
    const td = <td key={i}>{result.get(i)}</td>;
    tds.push(td);
  }
  return (
    <div className="container">
      <div className="row justify-content-center">
        <div className="col-md-8">
          <h4>Your favorite car is</h4>
          <table className="table table-hover text-center">
            <thead>
              <tr>
                {ths}
              </tr>
            </thead>
            <tbody>
              <tr>
                {tds}
              </tr>
            </tbody>
          </table>
        </div>
      </div>
    </div>
  )
}

const mapStateToProps = ({result, attributes, mask}) => ({
  result,
  attributes,
  mask
})

export default connect(mapStateToProps)(Result);