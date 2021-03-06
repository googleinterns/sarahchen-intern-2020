import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { DashboardComponent } from './dashboard.component';
import { HttpClientModule } from '@angular/common/http';

describe('DashboardComponent', () => {
  let component: DashboardComponent;
  let fixture: ComponentFixture<DashboardComponent>;
  const mockJSONInput = '{"SPAM":[["5/19/2020",true],["5/18/2020",false]]}';

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ DashboardComponent ],
      imports: [
        HttpClientModule
      ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(DashboardComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create the app', () => {
    expect(component).toBeTruthy();
  });

  it('should render dashboard', () => {
    let dashboard = fixture.nativeElement.querySelector('table');
    const input = JSON.parse(mockJSONInput);
    component.renderTemplate(input);
    // dashboard should not be displayed as long as both showChipper and
    // showChipperGDPR are false.
    expect(dashboard).toBeFalsy();
    component.showChipper = true;
    fixture.detectChanges();
    // Need to reselect dashboard as it should display the entire table now.
    dashboard = fixture.nativeElement.querySelector('table');
    expect(dashboard).toBeTruthy();
    const sourceTypeElement = fixture.nativeElement.querySelector('td');
    const firstDateElement = fixture.nativeElement.querySelector('td[title="5/19/2020"]');
    const secondDateElement = fixture.nativeElement.querySelector('td[title="5/18/2020"]');
    expect(sourceTypeElement.textContent).toEqual(' SPAM ');
    // Only one of online and offline should be online for either elements.
    expect(firstDateElement.querySelector('.online')).toBeTruthy();
    expect(firstDateElement.querySelector('.offline')).toBeFalsy();
    expect(secondDateElement.querySelector('.offline')).toBeTruthy();
    expect(secondDateElement.querySelector('.online')).toBeFalsy();
  });

  it('should set vars based on user clicks', () =>{
    expect(component.showChipper).toBe(false);
    expect(component.showChipperGDPR).toBe(false);
    // First clicking either buttons.
    component.onClick('Chipper');
    expect(component.showChipper).toBe(true);
    expect(component.showChipperGDPR).toBe(false);
    // Switching between dashboards.
    component.onClick('Chipper_GDPR');
    expect(component.showChipper).toBe(false);
    expect(component.showChipperGDPR).toBe(true);
    // Reclicking the same dashboard to hide it.
    component.onClick('Chipper_GDPR');
    expect(component.showChipper).toBe(false);
    expect(component.showChipperGDPR).toBe(false);
  });
});
