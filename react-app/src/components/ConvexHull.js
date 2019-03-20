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
      1000
    )
    this.controls = new THREE.OrbitControls(this.camera, this.mount);
    this.camera.position.set(1.5 , 1.5 , 1.5 );
    this.controls.update();

    this.renderer = new THREE.WebGLRenderer({ antialias: true });
    this.renderer.setClearColor(0xffffff);
    this.renderer.setSize(width, height);
    this.mount.appendChild(this.renderer.domElement);
    this.scene.add(new THREE.AxesHelper(1));

    this.start();
  }

  drawGeometry = () => {
    if (this.props.vertices < 4) return;
    const points = this.props.vertices.map(vertex => new THREE.Vector3(...vertex));
    const geometry = new THREE.ConvexGeometry(points);
    const material = new THREE.MeshBasicMaterial({
      color: 0xff0000,
      //shading: THREE.FlatShading,
      polygonOffset: true,
      polygonOffsetFactor: 1, // positive value pushes polygon further away
      polygonOffsetUnits: 1,
      opacity: 0.5,
      transparent: true
    });
    this.mesh = new THREE.Mesh(geometry, material);
    const geo = new THREE.EdgesGeometry(geometry); // or WireframeGeometry
    const mat = new THREE.LineBasicMaterial({ color: 0x000000, linewidth: 2 });
    const wireframe = new THREE.LineSegments(geo, mat);
    this.mesh.add(wireframe);
    this.scene.add(this.mesh);
  }

  componentDidUpdate(prevProps) {
    if (this.props.vertices !== prevProps.vertices) {
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
      <div
        style={{ width: '400px', height: '400px', margin: 'auto' }}
        ref={(mount) => { this.mount = mount }}
      />
    )
  }
}

const mapStateToProps = ({ vertices }) => ({ vertices });

export default connect(mapStateToProps)(ConvexHull);