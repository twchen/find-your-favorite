function parsePoints(text) {
  const points = text
    .trim()
    .split("\n")
    .slice(1) // the first line is <numOfPoints> <dimension>
    .map(line =>
      line
        .trim()
        .split(/\s+/)
        .map(str => parseFloat(str))
    );
  return points;
}

function parseLabels(text) {
  const labels = text.trim().split("\n");
  return labels;
}

function isInRange(val, low, high) {
  return low <= val && val <= high;
}

export const getRanges = points => {
  const ranges = points[0].map(x => ({ low: x, high: x }));
  points.slice(1).forEach(point => {
    point.forEach((x, i) => {
      ranges[i].low = Math.min(ranges[i].low, x);
      ranges[i].high = Math.max(ranges[i].high, x);
    });
  });
  return ranges;
};

export const loadDataset = async (pointsURL, labelsURL) => {
  let response = await fetch(pointsURL);
  const points = parsePoints(await response.text());
  if (labelsURL === undefined) {
    return points;
  }
  response = await fetch(labelsURL);
  const labels = parseLabels(await response.text());
  return [points, labels];
};

export const selectCandidates = (points, ranges, mask, maxPoints) => {
  const candidates = [];
  for (let i = 0; i < points.length; ++i) {
    if (candidates.length >= maxPoints) break;
    const point = points[i];
    let isValid = true;
    for (let j = 0; j < point.length; ++j) {
      if (mask[j] && !isInRange(point[j], ranges[j][0], ranges[j][1])) {
        isValid = false;
        break;
      }
    }
    if (isValid) candidates.push(point);
  }
  return candidates;
};

export const array2Vector2D = array => {
  const vector = new window.Module.VecVecDouble();
  array.forEach(arr => {
    const vec = new window.Module.VectorDouble();
    arr.forEach(x => vec.push_back(x));
    vector.push_back(vec);
  });
  return vector;
};

export const vector2Array2D = vector => {
  const array = [];
  for (let i = 0; i < vector.size(); ++i) {
    const vec = vector.get(i);
    const arr = [];
    for (let j = 0; j < vec.size(); ++j) arr.push(vec.get(j));
    array.push(arr);
  }
  return array;
};

export const vector2Array = vector => {
  const array = [];
  for (let i = 0; i < vector.size(); ++i) {
    array.push(vector.get(i));
  }
  return array;
};

export const getPrunedIndices = (prevIndices, currIndices) => {
  let prunedIndices = [];
  for (let i = 0, j = 0; i < prevIndices.size() || j < currIndices.size(); ) {
    if (j >= currIndices.size() || prevIndices.get(i) < currIndices.get(j)) {
      prunedIndices.push(prevIndices.get(i));
      ++i;
    } else {
      ++i;
      ++j;
    }
  }
  return prunedIndices;
};
