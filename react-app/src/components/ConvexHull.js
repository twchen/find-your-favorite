import React from 'react';
import { connect } from "react-redux";
import THREE from './three';

class ConvexHull extends React.Component {
  componentDidMount() {
    const width = this.mount.clientWidth
    const height = this.mount.clientHeight

    this.scene = new THREE.Scene()

    this.camera = new THREE.PerspectiveCamera(
      75,
      width / height,
      0.1,
      10
    )
    this.controls = new THREE.OrbitControls(this.camera, this.mount);
    this.camera.position.set(1.5, 1.5, 1.5);
    this.controls.update();

    this.renderer = new THREE.WebGLRenderer({ antialias: true });
    this.renderer.setClearColor('#cccccc');
    this.renderer.setSize(width, height);
    this.mount.appendChild(this.renderer.domElement);
    this.scene.add(new THREE.AxesHelper(1.5));
    this.start();
  }

  getRanges = () => {
    this.ranges = [
      [Infinity, 0],
      [Infinity, 0],
      [Infinity, 0]
    ];

    this.props.leftPoints.forEach(tuple => {
      tuple.slice(1).forEach((attr, i) => {
        if (this.ranges[i][0] > attr) {
          this.ranges[i][0] = attr;
        }
        if (this.ranges[i][1] < attr) {
          this.ranges[i][1] = attr;
        }
      });
    });
  }

  drawGeometry = () => {
    if (this.props.leftPoints.length < 2) return;
    if (this.ranges === undefined) this.getRanges();
    const points = this.props.leftPoints.map(tuple => {
      const attrs_scaled = tuple.slice(1).map((attr, i) => {
        const [min, max] = this.ranges[i];
        return (attr - min) / (max - min);
      });
      return new THREE.Vector3(...attrs_scaled);
    });

    if (points.length > 3) {
      const geometry = new THREE.ConvexGeometry(points);
      const material = new THREE.MeshBasicMaterial({ color: 0x336699, wireframe: true });
      this.mesh = new THREE.Mesh(geometry, material);
    } else {
      const geometry = new THREE.Geometry();
      points.forEach(point => {
        geometry.vertices.push(point);
      });
      if (points.length === 3) {
        // line geometry is not closed. Add the first point to close the line.
        geometry.vertices.push(points[0]);
      }
      const material = new THREE.LineBasicMaterial({ color: 0x336699 });
      this.mesh = new THREE.Line(geometry, material);
    }
    this.scene.add(this.mesh);
  }

  componentDidUpdate(prevProps) {
    if (this.props.leftPoints !== prevProps.leftPoints) {
      if (this.mesh) {
        this.scene.remove(this.mesh);
        this.mesh.geometry.dispose();
        this.mesh.material.dispose();
      }
      this.drawGeometry();
    }
  }

  componentWillUnmount() {
    this.stop();
    this.mount.removeChild(this.renderer.domElement);
  }

  start = () => {
    if (!this.frameId) {
      this.frameId = window.requestAnimationFrame(this.animate);
    }
  }

  stop = () => {
    window.cancelAnimationFrame(this.frameId)
  }

  animate = () => {
    this.controls.update();
    this.renderer.render(this.scene, this.camera);
    this.frameId = window.requestAnimationFrame(this.animate);
  }

  render() {
    return (
      <div style={{ margin: '1rem' }}>
        <h4>Convex Hull of Left Cars</h4>
        <div
          style={{ width: '400px', height: '400px', margin: 'auto' }}
          ref={(mount) => { this.mount = mount }}
        />
      </div>
    )
  }
}

const mapStateToProps = ({ candidates, leftPoints }) => ({ candidates, leftPoints });

export default connect(mapStateToProps)(ConvexHull);